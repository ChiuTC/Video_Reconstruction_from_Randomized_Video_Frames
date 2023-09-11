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
	vector<float> hist;													//�ĴX�i��  �ĴX�ӳq�D  �Ϥ���T
	int i, j;
	int imgNum = 1;														//�Ϲ���
	int histDim = 1;													//����Ϻ���
	int histSize[] = { 256 };											//����ϨC�@����bin�Ӽ�
	float rangeV[] = { 0, 256 };										//�C�@���ת��έp�d��
	const float* range[] = { rangeV };									//�p��d��    �]��V��RGB��255 �ҥH���i����0~256


	cv::Mat img_channel[3], avg_hist;										// �w�q���ΤT�q�� �P ����
	float number_hist = 0, avg_img_mean = 0;							//�w�q �s�F�h�ֱi��@����hist  �P �����Ƭ��h��

	for (auto& img : images) {
		cv::Mat tmp_img, tmp_hist; int flag_hist = 0;						//�w�q�Ȧs�Ϥ� �P hist  �٦�flag_hist(�T�{�O�_��HIST)
		tmp_img = img.clone();
		cvtColor(tmp_img, tmp_img, cv::COLOR_BGR2HSV);
		split(tmp_img, img_channel);
		calcHist(&img_channel[2], imgNum, 0, cv::Mat(), tmp_hist, histDim, histSize, range); //�p��G�ת�host�æs��tmp_hist����
//#pragma omp parallel for
		for (i = 0; i < tmp_hist.rows * tmp_hist.cols; i++) { tmp_hist.at<float>(i) /= (img.rows * img.cols); }	//�N�Ӽ��ഫ�����v
		//cout << tmp_hist.at<float>(255) << endl;
		//cout << tmp_hist.at<float>(0) << endl;

		double tmp_sum = 0;												//�T�O�G�פ������b���� �ӫD���k�ⰼ
//#pragma omp parallel for
		for (i = 50; i < 206; i++) { tmp_sum += tmp_hist.at<float>(i); }
		//cout << tmp_sum << endl;
		//cout << "$%$%$" << endl;																//�p�G�n�������x�}������ �B�j�����C�ⳣ�b���� �B �ⰼ(�t�ΫG) �èS����X
		if ((avg_hist.empty()) && (tmp_sum > 0.5) && (tmp_hist.at<float>(0) < 0.04) && (tmp_hist.at<float>(255) < 0.04) || (avg_hist.empty()) && (tmp_sum > 0.3) && (tmp_hist.at<float>(0) < 0.09) && (tmp_hist.at<float>(255) < 0.5)) {
			avg_hist = tmp_hist;
			number_hist += 1;
			flag_hist = 1;
			//cout << "!@!#" << endl;
		}																//�p�G �j�����C�ⳣ�b���� �B �ⰼ(�t�ΫG) �èS����X (��W���u�O�t�b�Ĥ@�ӭn���s�J�~��+=)
		else if ((tmp_sum > 0.5) && (tmp_hist.at<float>(0) < 0.04) && (tmp_hist.at<float>(255) < 0.04) || ((tmp_sum > 0.3) && (tmp_hist.at<float>(0) < 0.09) && (tmp_hist.at<float>(255) < 0.5))) {
			avg_hist += tmp_hist;
			number_hist += 1;
			flag_hist = 1;
			//cout << "!@!#" << endl;
		}
		tmp_sum = 0;													//�p��o�i�Ϥ����G�ץ�����
		for (i = 0; i < 256; i++) { tmp_sum += tmp_hist.at<float>(i) * i; }
		if (flag_hist == 1) { avg_img_mean += tmp_sum; }				//�p�G�n�O���������G�׭� �h�N�G�ץ����[�_��
		hist.push_back(tmp_sum);										//�N���i�Ϫ��G�ץ��� �s���T�̭�
	}

	avg_img_mean /= number_hist;										//�j��~ �ҥH�i�H������Ӹ�Ƨ��� �n���Ϥ��G�����ӭn�h��

	//cout << "����meam: " << avg_img_mean << endl;
//#pragma omp parallel for
	for (int j = 0; j < images.size(); j++) {							//�o��}�l��Ҧ��Ϥ����B�z

		//printf("%d\n", j);
		//cout << "==============" << endl;

		float dif = hist[j] - avg_img_mean;								//��X�o�i�Ϥ��������G�׻P ���Ȯt�h��
		//cout << "�t�Ȭ��G" << dif << endl;
		cv::Mat img = images[j].clone();
		cv::Mat img_channel[3];

		if (dif < -30 || dif>80) {
			cvtColor(img, img, cv::COLOR_BGR2YCrCb);


		}
		split(img, img_channel);

		if (dif > 80) {
			cv::Mat operate(img.rows, img.cols, CV_8UC1, 10 * exp(fabs(dif) / 65));
			img_channel[0] = img_channel[0] - operate;
			//cout << "��" << j << "�i�Ϫ�power�G" << exp(0.4 * fabs(dif) / 60) << endl;
			change_gamma(dif, 1, img_channel[0], img_channel[0], 1);

		}
		else if (dif > 0) {  //�p�G�t�Ȥj��s �N�� ��Ϥ���G �ҥH�C�ӳq�D�����C�o�Ӯt�Ȫ��G�� �Ϩ�򥭧��@��
//#pragma omp parallel for
			for (i = 0; i < 3; i++) {
				cv::Mat operate(img.rows, img.cols, CV_8UC1, dif);
				img_channel[i] = img_channel[i] - operate;
			}
		}
		else if (dif<0 && dif>-30) {									//�p�G�t�Ƚd�����p �B�b0~-40��  �i�H�ϥΥ[�k���Ϥ��ܫG
//#pragma omp parallel for
			for (i = 0; i < 3; i++) {
				cv::Mat operate(img.rows, img.cols, CV_8UC1, fabs(dif));
				img_channel[i] = img_channel[i] + operate;
			}
		}
		else {															//�p�G�t�ȤӤj �ݭn��power law ���C����}
			//cout << "��" << j << "�i�Ϫ�power�G" << (1 - exp(log(1 + fabs(dif)) / 3) / 10) << endl;
//#pragma omp parallel for
			for (i = 0; i < 1; i++) {


				//cout << img_channel[i] << endl;
				change_gamma(dif, 1, img_channel[i], img_channel[i], 0);
				cv::Mat operate(img.rows, img.cols, CV_8UC1, 10 * exp(fabs(dif) / 85));
				img_channel[i] = img_channel[i] + operate;
			}
		}
		//imshow("���", img);
		cv::Mat result;
		merge(img_channel, 3, result);
		if (dif < -30 || dif>80) {
			cvtColor(result, result, cv::COLOR_YCrCb2BGR);
		}
		result.copyTo(images[j]);
		//imshow("���᪺", result);
		//waitKey(0);

	}


}
int brightnessBalance::checkimg(std::vector<cv::Mat> images) {
	std::vector<float> hist;													//�ĴX�i��  �ĴX�ӳq�D  �Ϥ���T
	int i, j;
	int imgNum = 1;														//�Ϲ���
	int histDim = 1;													//����Ϻ���
	int histSize[] = { 256 };											//����ϨC�@����bin�Ӽ�
	float rangeV[] = { 0, 256 };										//�C�@���ת��έp�d��
	const float* range[] = { rangeV };									//�p��d��    �]��V��RGB��255 �ҥH���i����0~256


	cv::Mat img_channel[3], avg_rgb_hist[3];										// �w�q���ΤT�q�� �P ����
	float number_hist = 0, avg_img_mean = 0;							//�w�q �s�F�h�ֱi��@����hist  �P �����Ƭ��h��
	for (auto& img : images) {
		cv::Mat tmp_img, tmp_hist[3]; int flag_hist = 0;						//�w�q�Ȧs�Ϥ� �P hist  �٦�flag_hist(�T�{�O�_��HIST)
		tmp_img = img.clone();

		split(tmp_img, img_channel);
		for (int i = 0; i < 3; i++) {
			calcHist(&img_channel[i], imgNum, 0, cv::Mat(), tmp_hist[i], histDim, histSize, range); //�p��G�ת�host�æs��tmp_hist����
			for (j = 0; j < tmp_hist[i].rows * tmp_hist[i].cols; j++) { tmp_hist[i].at<float>(j) /= (img.rows * img.cols); }	//�N�Ӽ��ഫ�����v
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
	//std::cout << "    avg�G" << avg << std::endl;
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