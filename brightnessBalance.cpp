#include "brightnessBalance.h"

//using namespace cv;
void brightnessBalance::change_gamma(double dif, double c, cv::Mat input_img, cv::Mat output_img, int flag) {
	float r;
	int i, j;
	cv::Mat tmp_input = input_img.clone();
	for (i = 0; i < input_img.rows; i++)
	{
		for (j = 0; j < input_img.cols; j++) {


			if (flag == 0) {
				double gamma = (1 - exp(log(1 + fabs(dif)) / 3) / 10);
				r = tmp_input.at<uchar>(i, j);
				r /= 255;
				int tmp = lround(255 * c * pow(r, gamma));
				if (tmp > 255) { output_img.at<uchar>(i, j) = 255; }
				else if (tmp < 0) { output_img.at<uchar>(i, j) = 0; }
				else { output_img.at<uchar>(i, j) = tmp; }
			}
			else {

				r = tmp_input.at<uchar>(i, j);
				double gamma = exp(0.4 * fabs(dif) / 60);
				r /= 255;
				int tmp = lround(255 * c * pow(r, gamma));
				if (tmp > 255) { output_img.at<uchar>(i, j) = 255; }
				else if (tmp < 0) { output_img.at<uchar>(i, j) = 0; }
				else { output_img.at<uchar>(i, j) = tmp; }
			}
		}
	}

}
void brightnessBalance::transfor_img(vector<cv::Mat> images) {
	vector<float> hist;													//第幾張圖  第幾個通道  圖片資訊
	int i, j;
	int imgNum = 1;														//圖像數
	int histDim = 1;													//直方圖維度
	int histSize[] = { 256 };											//直方圖每一維度bin個數
	float rangeV[] = { 0, 256 };										//每一維度的統計範圍
	const float* range[] = { rangeV };									//計算範圍    因為V跟RGB都255 所以偷懶都用0~256


	cv::Mat img_channel[3], avg_hist;										// 定義分割三通到 與 平均
	float number_hist = 0, avg_img_mean = 0;							//定義 存了多少張當作平均hist  與 平均數為多少

	for (auto& img : images) {
		cv::Mat tmp_img, tmp_hist; int flag_hist = 0;						//定義暫存圖片 與 hist  還有flag_hist(確認是否有HIST)
		tmp_img = img.clone();
		cvtColor(tmp_img, tmp_img, cv::COLOR_BGR2HSV);
		split(tmp_img, img_channel);
		calcHist(&img_channel[2], imgNum, 0, cv::Mat(), tmp_hist, histDim, histSize, range); //計算亮度的host並存到tmp_hist內部
//#pragma omp parallel for
		for (i = 0; i < tmp_hist.rows * tmp_hist.cols; i++) { tmp_hist.at<float>(i) /= (img.rows * img.cols); }	//將個數轉換成機率
		//cout << tmp_hist.at<float>(255) << endl;
		//cout << tmp_hist.at<float>(0) << endl;

		double tmp_sum = 0;												//確保亮度分布都在中間 而非左右兩側
//#pragma omp parallel for
		for (i = 50; i < 206; i++) { tmp_sum += tmp_hist.at<float>(i); }
		//cout << tmp_sum << endl;
		//cout << "$%$%$" << endl;																//如果要平均的矩陣內為空 且大部分顏色都在中間 且 兩側(暗或亮) 並沒有突出
		if ((avg_hist.empty()) && (tmp_sum > 0.5) && (tmp_hist.at<float>(0) < 0.04) && (tmp_hist.at<float>(255) < 0.04) || (avg_hist.empty()) && (tmp_sum > 0.3) && (tmp_hist.at<float>(0) < 0.09) && (tmp_hist.at<float>(255) < 0.5)) {
			avg_hist = tmp_hist;
			number_hist += 1;
			flag_hist = 1;
			//cout << "!@!#" << endl;
		}																//如果 大部分顏色都在中間 且 兩側(暗或亮) 並沒有突出 (跟上面只是差在第一個要先存入才能+=)
		else if ((tmp_sum > 0.5) && (tmp_hist.at<float>(0) < 0.04) && (tmp_hist.at<float>(255) < 0.04) || ((tmp_sum > 0.3) && (tmp_hist.at<float>(0) < 0.09) && (tmp_hist.at<float>(255) < 0.5))) {
			avg_hist += tmp_hist;
			number_hist += 1;
			flag_hist = 1;
			//cout << "!@!#" << endl;
		}
		tmp_sum = 0;													//計算這張圖片的亮度平均值
		for (i = 0; i < 256; i++) { tmp_sum += tmp_hist.at<float>(i) * i; }
		if (flag_hist == 1) { avg_img_mean += tmp_sum; }				//如果要記錄此側的亮度值 則將亮度平均加起來
		hist.push_back(tmp_sum);										//將此張圖的亮度平均 存到資訊裡面
	}

	avg_img_mean /= number_hist;										//迴圈外 所以可以平均整個資料夾內 好的圖片亮度應該要多少

	//cout << "平均meam: " << avg_img_mean << endl;
//#pragma omp parallel for
	for (int j = 0; j < images.size(); j++) {							//這邊開始對所有圖片做處理

		//printf("%d\n", j);
		//cout << "==============" << endl;

		float dif = hist[j] - avg_img_mean;								//找出這張圖片的平均亮度與 均值差多少
		//cout << "差值為：" << dif << endl;
		cv::Mat img = images[j].clone();
		cv::Mat img_channel[3];

		if (dif < -30 || dif>80) {
			cvtColor(img, img, cv::COLOR_BGR2YCrCb);


		}
		split(img, img_channel);

		if (dif > 80) {
			cv::Mat operate(img.rows, img.cols, CV_8UC1, 10 * exp(fabs(dif) / 65));
			img_channel[0] = img_channel[0] - operate;
			//cout << "第" << j << "張圖的power：" << exp(0.4 * fabs(dif) / 60) << endl;
			change_gamma(dif, 1, img_channel[0], img_channel[0], 1);

		}
		else if (dif > 0) {  //如果差值大於零 代表 原圖比較亮 所以每個通道都降低這個差值的亮度 使其跟平均一樣
//#pragma omp parallel for
			for (i = 0; i < 3; i++) {
				cv::Mat operate(img.rows, img.cols, CV_8UC1, dif);
				img_channel[i] = img_channel[i] - operate;
			}
		}
		else if (dif<0 && dif>-30) {									//如果差值範圍比較小 且在0~-40間  可以使用加法讓圖片變亮
//#pragma omp parallel for
			for (i = 0; i < 3; i++) {
				cv::Mat operate(img.rows, img.cols, CV_8UC1, fabs(dif));
				img_channel[i] = img_channel[i] + operate;
			}
		}
		else {															//如果差值太大 需要用power law 讓顏色分開
			//cout << "第" << j << "張圖的power：" << (1 - exp(log(1 + fabs(dif)) / 3) / 10) << endl;
//#pragma omp parallel for
			for (i = 0; i < 1; i++) {


				//cout << img_channel[i] << endl;
				change_gamma(dif, 1, img_channel[i], img_channel[i], 0);
				cv::Mat operate(img.rows, img.cols, CV_8UC1, 10 * exp(fabs(dif) / 85));
				img_channel[i] = img_channel[i] + operate;
			}
		}
		//imshow("原圖", img);
		cv::Mat result;
		merge(img_channel, 3, result);
		if (dif < -30 || dif>80) {
			cvtColor(result, result, cv::COLOR_YCrCb2BGR);
		}
		result.copyTo(images[j]);
		//imshow("更改後的", result);
		//waitKey(0);

	}


}
int brightnessBalance::checkimg(std::vector<cv::Mat> images) {
	std::vector<float> hist;													//第幾張圖  第幾個通道  圖片資訊
	int i, j;
	int imgNum = 1;														//圖像數
	int histDim = 1;													//直方圖維度
	int histSize[] = { 256 };											//直方圖每一維度bin個數
	float rangeV[] = { 0, 256 };										//每一維度的統計範圍
	const float* range[] = { rangeV };									//計算範圍    因為V跟RGB都255 所以偷懶都用0~256


	cv::Mat img_channel[3], avg_rgb_hist[3];										// 定義分割三通到 與 平均
	float number_hist = 0, avg_img_mean = 0;							//定義 存了多少張當作平均hist  與 平均數為多少
	for (auto& img : images) {
		cv::Mat tmp_img, tmp_hist[3]; int flag_hist = 0;						//定義暫存圖片 與 hist  還有flag_hist(確認是否有HIST)
		tmp_img = img.clone();

		split(tmp_img, img_channel);
		for (int i = 0; i < 3; i++) {
			calcHist(&img_channel[i], imgNum, 0, cv::Mat(), tmp_hist[i], histDim, histSize, range); //計算亮度的host並存到tmp_hist內部
			for (j = 0; j < tmp_hist[i].rows * tmp_hist[i].cols; j++) { tmp_hist[i].at<float>(j) /= (img.rows * img.cols); }	//將個數轉換成機率
			if (avg_rgb_hist[i].empty()) {
				avg_rgb_hist[i] = tmp_hist[i];
			}
			else {
				avg_rgb_hist[i] += tmp_hist[i];
			}
		}

	}
	for (i = 0; i < 3; i++) {
		avg_rgb_hist[i] /= images.size();
	}
	/*std::cout << "b0: " << avg_rgb_hist[0].at<float>(0) << "  b255: " << avg_rgb_hist[0].at<float>(255) << std::endl;
	std::cout << "g0: " << avg_rgb_hist[1].at<float>(0) << "  g255: " << avg_rgb_hist[1].at<float>(255) << std::endl;
	std::cout << "r0: " << avg_rgb_hist[2].at<float>(0) << "  r255: " << avg_rgb_hist[2].at<float>(255) << std::endl;*/



	double sum_rgb[3];
	sum_rgb[0] = 0; sum_rgb[1] = 0; sum_rgb[2] = 0;


	sum_rgb[0] = 0; sum_rgb[1] = 0; sum_rgb[2] = 0;
	for (j = 0; j < 3; j++) {
		for (i = 0; i < 256; i++) {
			sum_rgb[j] += avg_rgb_hist[j].at<float>(i) * i;
		}
	}


	//std::cout << "b: " << sum_rgb[0];
	//std::cout << "    g: " << sum_rgb[1];
	//std::cout << "    r: " << sum_rgb[2];
	double avg = (sum_rgb[0] + sum_rgb[1] + sum_rgb[2]) / 3;
	//std::cout << "    avg：" << avg << std::endl;
	int flag_1 = 0, flag_128 = 0, flag_avg = 0, flag_ga = 0;
	if (avg_rgb_hist[0].at<float>(0) > 0.01 && avg_rgb_hist[1].at<float>(0) > 0.01 && avg_rgb_hist[2].at<float>(0) > 0.01
		&& avg_rgb_hist[0].at<float>(255) > 0.01 && avg_rgb_hist[1].at<float>(255) > 0.01 && avg_rgb_hist[2].at<float>(255) > 0.01) {
		flag_1 = 1;
	}
	if (sum_rgb[0] > 108 && sum_rgb[0] < 148 && sum_rgb[1] > 108 && sum_rgb[1] < 148
		&& sum_rgb[2] > 108 && sum_rgb[2] < 148) {
		flag_128 = 1;
	}
	if (avg > 108 && avg < 148) {
		flag_avg = 1;
	}
	if (avg_rgb_hist[0].at<float>(0) > 0.035 && avg_rgb_hist[1].at<float>(0) > 0.035 && avg_rgb_hist[2].at<float>(0) > 0.035
		&& avg_rgb_hist[0].at<float>(255) > 0.035 && avg_rgb_hist[1].at<float>(255) > 0.035 && avg_rgb_hist[2].at<float>(255) > 0.035) {
		flag_ga = 1;
	}

	if (flag_1 == 1 && flag_128 == 1 && flag_avg == 1) {
		if (flag_ga == 1) { return 1; }
		else { return 2; }
	}
	else {
		return 0;
	}
}