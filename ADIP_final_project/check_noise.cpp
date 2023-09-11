//#include"QmoHeader.h"
//#include<time.h>
//#include "CF.h"
#include <iostream>
#include <opencv2/opencv.hpp>
//std::string inputimg_path;   //<-----------定義輸入資料夾
int checkimg(std::vector<cv::Mat> images) {
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
	std::cout << "b0: " << avg_rgb_hist[0].at<float>(0) << "  b255: " << avg_rgb_hist[0].at<float>(255) << std::endl;
	std::cout << "g0: " << avg_rgb_hist[1].at<float>(0) << "  g255: " << avg_rgb_hist[1].at<float>(255) << std::endl;
	std::cout << "r0: " << avg_rgb_hist[2].at<float>(0) << "  r255: " << avg_rgb_hist[2].at<float>(255) << std::endl;



	double sum_rgb[3];
	sum_rgb[0] = 0; sum_rgb[1] = 0; sum_rgb[2] = 0;


	sum_rgb[0] = 0; sum_rgb[1] = 0; sum_rgb[2] = 0;
	for (j = 0; j < 3; j++) {
		for (i = 0; i < 256; i++) {
			sum_rgb[j] += avg_rgb_hist[j].at<float>(i)*i;
		}
	}


	std::cout << "b: " << sum_rgb[0];
	std::cout << "    g: " << sum_rgb[1] ;
	std::cout << "    r: " << sum_rgb[2];
	double avg = (sum_rgb[0] + sum_rgb[1] + sum_rgb[2])/3;
	std::cout << "    avg：" << avg << std::endl;
	int flag_1 = 0 , flag_128=0,flag_avg=0,flag_ga=0;
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

