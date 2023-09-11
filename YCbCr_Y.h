#undef UNICODE

#include"windows.h"
#include<shlobj.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include <string>
#include <vector>
#include <time.h>

#include<opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>   // for imshow
#include <opencv2/imgcodecs.hpp> // for imread

using namespace cv;
using namespace std;
//string inputimg_path;   //<-----------定義輸入資料夾

string getpath();
void load_data(string& , vector<Mat>& );

class YCbCr_Y {
private:
	
public:
	vector<cv::Mat> output_img_p;
	vector<cv::Mat> output_img_c;
	void power_law(vector<cv::Mat>);
	void Correction(vector<cv::Mat>);
};

//class HSV {
//public:
//	void power_law(vector<cv::Mat>&, vector<cv::Mat>&);
//	void Correction(vector<cv::Mat>&, vector<cv::Mat>&);
//};
//
//class PUBG {
//public:
//	void HSL_Median();
//	void pow_hist(cv::Mat&);
//	void im_show(const char* , const Mat& );
//};
//
//void DCT();
//void test();
//void hist(cv::Mat&);
//void test_Dark_Channel();