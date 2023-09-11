#pragma once
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <fstream>
#include <algorithm>
#include <omp.h>
using namespace std;

class FrameProcess {
private:
	//adaptive parameters
	vector<cv::Mat> temp_image_arr;
	//input image & txt order
	vector<cv::Mat> image_arr;
	vector<cv::Mat> image_arr_gray;
	vector<uint16_t> groud_truth_order;
	vector<cv::Mat> groud_truth_arr;
	//calKeyPoints
	vector<vector<cv::KeyPoint>> frames_keyPoint;
	vector<cv::Mat> frames_desp;
	//calMatch
	struct sort_frames_info {
		sort_frames_info();
		~sort_frames_info();
		unsigned short* frames_good_matches_pair;
		unsigned short max_good_matches, min_good_matches;
		unsigned int average_keyPoints;;
		unsigned int average_matches;
		vector<bool> already_add;
		cv::Point max_match_index;
	};
	sort_frames_info sort_info;
	vector<cv::DMatch> good_matches;
	vector<vector<cv::DMatch>> knn_matches;
	const float ratio_thresh = 0.6f;
	//sort
	vector<uint16_t> our_output_order;
	vector<uint16_t> our_output_reorder;
	vector<cv::Mat> our_output_arr;
	//feature detector
	cv::Ptr<cv::xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create(1);
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("FlannBased");
	bool inv = false;
public:
	double g_total_SRCC = 0.0f;
	double g_total_MSE = 0.0f;
	double g_total_time = 0.0f;
	double g_total_sort_time = 0.0f;
	double g_total_pre_time = 0.0f;
	double g_total_match_time = 0.0f;
	double end_pre_time = 0.0f;
	int what_noise = 0;
	bool do_median = false;
	bool first = true;
	FrameProcess();
	~FrameProcess();
	int reconstrutFrame(char folder[], double resize_ratio, int HessianThreshold, uint speed_up, bool enforce, int i, bool do_m, vector<cv::Mat>& temp_img_arr, bool& first, int& what_n, char name[]);
	void calKeyPoints(int input_frame_num, int HessianThreshold);
	void calMatch(int input_frame_num, int HessianThreshold);
	void sort(int input_frame_num);
	void run(char folder[], int i, char name[]);
	double SRCC(vector<uint16_t> groud_truth, vector<uint16_t> our_output);
	double MSE(vector<cv::Mat> groud_truth, vector<uint16_t> our_output_reorder);
};