#pragma once
#include <iostream>
#include <cstdio>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
using namespace std;
class brightnessBalance {
private:
public:
	void change_gamma(double dif, double c, cv::Mat input_img, cv::Mat output_img, int flag);
	void transfor_img(vector<cv::Mat> images);
	int checkimg(std::vector<cv::Mat> images);
};