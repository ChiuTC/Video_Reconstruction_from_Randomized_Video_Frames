#include "FrameProcess.h"
#include <time.h>
#include "brightnessBalance.h"
#include "YCbCr_Y.h"
#include"CF.h"
using namespace std;
//using namespace cv;

FrameProcess::FrameProcess() {
	good_matches.reserve(100);
	knn_matches.reserve(100);
	groud_truth_order.reserve(500);
	our_output_order.reserve(500);
}

FrameProcess::~FrameProcess() {
}
FrameProcess::sort_frames_info::~sort_frames_info() {
	delete[] frames_good_matches_pair;
}

FrameProcess::sort_frames_info::sort_frames_info() {
	max_good_matches = 0;
	min_good_matches = 10000;
	average_matches = 0;
	average_keyPoints = 0;
	max_match_index.x = 0;
	max_match_index.y = 0;
}

void FrameProcess::calKeyPoints(int input_frame_num, int HessianThreshold) {
	surf->setHessianThreshold(HessianThreshold);
	unsigned int total_keyPoint = 0;
	unsigned int total_cal_num = 0;

#pragma omp parallel for
	for (int i = 0; i < image_arr_gray.size(); i++) {
		vector<cv::KeyPoint> keyPoint;
		cv::Mat desp;
		keyPoint.reserve(1000);
		surf->detectAndCompute(image_arr_gray[i], cv::noArray(), keyPoint, desp);
		frames_keyPoint[i] = keyPoint;
		frames_desp[i] = desp;
		total_keyPoint += keyPoint.size();
		total_cal_num += 1;
	}
	sort_info.average_keyPoints = total_keyPoint / total_cal_num;
}
void FrameProcess::calMatch(int input_frame_num, int HessianThreshold) {
	int i = 0;
	unsigned short good_matches_num = 0;
	unsigned int total_matches = 0;
	unsigned int total_cal_num = 0;
	sort_info.frames_good_matches_pair = new unsigned short[input_frame_num * input_frame_num * sizeof(unsigned short)];
	ofstream fout("match_order.txt");
	int total = 0;

#pragma omp parallel for ordered
	for (int x = 0; x < input_frame_num; x++) {
		total += 1;
		for (int y = 0; y < input_frame_num; y++) {
			vector<cv::DMatch> good_matches;
			vector<vector<cv::DMatch>> knn_matches;

			if (x == y) {
				sort_info.frames_good_matches_pair[x * input_frame_num + y] = -1;
				fout << x << "," << y << ":" << -1 << endl;
				continue;
			}
			if (frames_desp[x].type() != CV_32F || frames_desp[y].type() != CV_32F) {
				frames_desp[x].convertTo(frames_desp[x], CV_32F);
				frames_desp[y].convertTo(frames_desp[y], CV_32F);
			}
			matcher->knnMatch(frames_desp[x], frames_desp[y], knn_matches, 2);

			for (size_t i = 0; i < knn_matches.size(); i++)
			{
				if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
				{
					good_matches.push_back(knn_matches[i][0]);
				}
			}

			good_matches_num = good_matches.size();
			total_matches += good_matches_num;
			total_cal_num += 1;
			sort_info.min_good_matches = min(sort_info.min_good_matches, good_matches_num);
			sort_info.max_good_matches = max(sort_info.max_good_matches, good_matches_num);
			sort_info.frames_good_matches_pair[x * input_frame_num + y] = good_matches_num;
			if (sort_info.max_good_matches > good_matches_num) {
				sort_info.max_match_index.x = x;
				sort_info.max_match_index.y = y;
			}

			knn_matches.clear();
			good_matches.clear();
			fout << x << "," << y << ":" << good_matches_num << endl;

		}
		printf("calMatch:%.2f%%\n", (float(total) / float(input_frame_num)) * 100);
	}
	sort_info.average_matches = total_matches / total_cal_num;
}
void FrameProcess::sort(int input_frame_num) {
	cv::Point match;
	cv::Point head_match;
	cv::Point end_match;
	int x = 0;
	int head, end;
	unsigned short max_val;
	unsigned short head_max_val;
	unsigned short end_max_val;
	int times = 0;
	sort_info.already_add.resize(input_frame_num, false);
	while (1) {
		if (our_output_order.size() < 2) {
			max_val = sort_info.min_good_matches;
			for (int y = 0; y < input_frame_num; y++) {
				if (x != y) {
					if (sort_info.frames_good_matches_pair[x * input_frame_num + y] > max_val) {
						max_val = sort_info.frames_good_matches_pair[x * input_frame_num + y];
						match.x = x;
						match.y = y;
					}
				}
			}
			our_output_order.push_back(match.x);
			our_output_order.push_back(match.y);
			our_output_arr.push_back(image_arr[match.x]);
			our_output_arr.push_back(image_arr[match.y]);
		}
		else {
			head = our_output_order[0];
			end = our_output_order[our_output_order.size() - 1];
			head_max_val = sort_info.min_good_matches;
			end_max_val = sort_info.min_good_matches;

			for (int y = 0; y < input_frame_num; y++) {
				if (head != y && y != end) {
					if (!sort_info.already_add[y]) {
						if (sort_info.frames_good_matches_pair[head * input_frame_num + y] > head_max_val) {
							head_max_val = sort_info.frames_good_matches_pair[head * input_frame_num + y];
							head_match.x = head;
							head_match.y = y;
						}
					}
				}
			}

			for (int y = 0; y < input_frame_num; y++) {
				if (end != y && y != head) {
					if (!sort_info.already_add[y]) {
						if (sort_info.frames_good_matches_pair[end * input_frame_num + y] > end_max_val) {
							end_max_val = sort_info.frames_good_matches_pair[end * input_frame_num + y];
							end_match.x = end;
							end_match.y = y;
						}
					}
				}
			}
			if (head_max_val > end_max_val) {
				sort_info.already_add[head] = true;
				our_output_order.insert(our_output_order.begin(), head_match.y);
			}
			else {
				sort_info.already_add[end] = true;
				our_output_order.push_back(end_match.y);
			}

			if (our_output_order.size() == input_frame_num) {
				break;
			}

		}
		times += 1;
	}



}

int FrameProcess::reconstrutFrame(char folder[], double resize_ratio, int HessianThreshold, uint speed_up, bool enforce, int i, bool do_m, vector<cv::Mat>& temp_img_arr, bool& first, int& what_n, char name[]) {
	brightnessBalance bribal;
	
	//time
	double START, END, START2, END2, START3, END3, START4, END4, sort_start, sort_end, match_start, match_end;
	START4 = clock();
	START = clock();
	//VideoWriter
	cv::VideoWriter writer;
	char image_name[300];
	string s_image_name;
	int isColor = 1;
	//use for reconstrutFrame
	int good_matches_num = 0;
	int next_img_index = 0;
	int temp = 0;
	int input_frame_num = 0;
	//get frame information
	sprintf_s(image_name, "%s%s%s", folder, "/random/", "001.png");
	//printf("%s\n", image_name);
	s_image_name = image_name;
	cv::Mat test_img = cv::imread(s_image_name);
	int frame_width = int(test_img.cols);
	int frame_height = int(test_img.rows);

	//read answer
	sprintf_s(image_name, "%s%s", folder, "/answer.txt");
	s_image_name = image_name;
	ifstream ifs(image_name, ios::in);
	string s;
	while (getline(ifs, s)) {
		groud_truth_order.push_back(stoul(s));
		input_frame_num += 1;
	}
	input_frame_num /= speed_up;

	
	frames_keyPoint.resize(input_frame_num);
	frames_desp.resize(input_frame_num);
	groud_truth_arr.resize(input_frame_num);
	our_output_arr.resize(input_frame_num);

	//read frames
	START2 = clock();
	image_arr_gray.resize(input_frame_num);
	image_arr.resize(input_frame_num);

#pragma omp parallel for
	for (int i = 0; i < input_frame_num; i++) {
		int H = 0, T = 0, U = 0;
		T = (i + 1) / 10;
		U = (i + 1) - 10 * T;
		H = T / 10;
		T = T - H * 10;
		cv::Mat image_temp;
		string s_image_name;
		char image_name[300];
		sprintf_s(image_name, "%s%s%d%d%d%s", folder, "/Ground truth/", H, T, U, ".png");
		//printf("%s\n", image_name);
		s_image_name = image_name;
		image_temp = cv::imread(s_image_name);

		if (!image_temp.data) {
			break;
		}
		groud_truth_arr[i] = image_temp;

		sprintf_s(image_name, "%s%s%d%d%d%s", folder, "/random/", H, T, U, ".png");
		s_image_name = image_name;
		image_temp = cv::imread(s_image_name);
		image_arr[i] = image_temp;

	}
	END2 = clock();

	//printf("load images 執行所花費時間:%f\n", double(clock() / CLOCKS_PER_SEC));
	//printf("load images  進行運算所花費時間:%f\n\n", (END2 - START2) / CLOCKS_PER_SEC);
	
	if (speed_up == 1) {
		what_n = bribal.checkimg(image_arr);
	}

	//default
	//printf("do default median filter\n");
	#pragma omp parallel for
	for (int i = 0; i < input_frame_num; i++) {
		//printf("do median filter\n");
		medianBlur(image_arr[i], image_arr[i], 5);//median filter
	}

	if (speed_up == 1) {
		
		if (what_n == 0) {
			printf("no noise\n");

			bribal.transfor_img(groud_truth_arr);
			/*YCbCr_Y ycbcr;
			ycbcr.power_law(groud_truth_arr);
			ycbcr.Correction(ycbcr.output_img_p);
			for (int i = 0; i < input_frame_num; i++) {
				groud_truth_arr[i] = ycbcr.output_img_c[i];
			}*/
		}
		else if (what_n == 1) {
			printf("Gaussian noise\n");
			printf("do curvature filter\n");
			do_CF(image_arr, 5, "t", 10, 2.2, 1.5);

		}
		else if (what_n == 2) {
			printf("pepper noise\n");
			printf("do median filter\n");
			for (int i = 0; i < input_frame_num; i++) {
				medianBlur(image_arr[i], image_arr[i], 5);
			}

		}
		//-----------------
		//color adjustment
		//(1)
		bribal.transfor_img(image_arr);
		//(2)
		/*YCbCr_Y ycbcr;
		ycbcr.power_law(image_arr);
		ycbcr.Correction(ycbcr.output_img_p);
		for (int i = 0; i < input_frame_num; i++) {
			image_arr[i] = ycbcr.output_img_c[i];
		}*/
		//-----------------
		#pragma omp parallel for
		for (int i = 0; i < input_frame_num; i++) {
			cvtColor(image_arr[i], image_arr_gray[i], cv::COLOR_BGR2GRAY);
			resize(image_arr_gray[i], image_arr_gray[i], cv::Size(frame_width / resize_ratio, frame_height / resize_ratio));
		}
	}

	if (speed_up != 1) {
		#pragma omp parallel for
		for (int i = 0; i < input_frame_num; i++) {
			cvtColor(image_arr[i], image_arr_gray[i], cv::COLOR_BGR2GRAY);
			resize(image_arr_gray[i], image_arr_gray[i], cv::Size(frame_width / resize_ratio, frame_height / resize_ratio));
		}
	}


	float frame_fps = input_frame_num / 15;
	//writer = cv::VideoWriter(video_name, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), frame_fps, cv::Size(frame_width, frame_height), isColor);

	
	calKeyPoints(input_frame_num, HessianThreshold);
	;
	if (sort_info.average_keyPoints >= 500 && sort_info.average_keyPoints <= 600 || enforce) {
		if (speed_up != 1) {
			return 0;
		}
		printf("input_frame_num = %d\n\n", input_frame_num);
		printf("frame_width = %d\n", frame_width);
		printf("frame_height = %d\n", frame_height);
		printf("frame_fps = %f\n\n", frame_fps);

		end_pre_time = clock();
		match_start = clock();
		calMatch(input_frame_num, HessianThreshold);
		match_end = clock();
		g_total_match_time += (match_end - match_start) / CLOCKS_PER_SEC;
		sort_start = clock();
		sort(input_frame_num);
		sort_end = clock();
		g_total_sort_time += (sort_end - sort_start) / CLOCKS_PER_SEC;
		
		//create a txt
		sprintf_s(image_name, "%s%s", name, "_sort.txt");
		ofstream fout(image_name);

		sprintf_s(image_name, "%s%s", name, ".avi");
		string video_name = image_name;
		writer = cv::VideoWriter(video_name, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), frame_fps, cv::Size(frame_width, frame_height), isColor);

		vector<bool> already_add;
		already_add.resize(input_frame_num);
		next_img_index = 0;
		int i = 0;
		int global_min_good_matches_num = 30000;
		cv::Point first_to_end(0, 0);

		END = clock();
		//printf("reconstrutFrame 執行所花費時間:%f\n", double(clock() / CLOCKS_PER_SEC));
		//printf("reconstrutFrame 進行運算所花費時間:%f\n\n", (END - START) / CLOCKS_PER_SEC);

		//output answer (video and txt)
		if (what_n == 1 || what_n == 2) {
			for (int i = 0; i < our_output_order.size(); i++) {
				writer.write(image_arr[our_output_order[i]]);
				our_output_arr[i] = image_arr[our_output_order[i]];
			}
		}

		//reorder
		our_output_reorder.resize(input_frame_num);
		for (int i = 0; i < our_output_reorder.size(); i++) {
			vector<uint16_t>::iterator it = find(groud_truth_order.begin(), groud_truth_order.end(), our_output_order[i] + 1);
			our_output_reorder[i] = distance(groud_truth_order.begin(), it) + 1;
			fout << our_output_reorder[i] << endl;
		}
		if (what_n == 0) {
			for (int i = 0; i < our_output_order.size(); i++) {
				writer.write(groud_truth_arr[our_output_reorder[i] - 1]);
			}
		}
		double SRCC_val = SRCC(groud_truth_order, our_output_reorder);
		g_total_SRCC += SRCC_val;

		START3 = clock();
		double MSE_val = MSE(groud_truth_arr, our_output_reorder);
		g_total_MSE += MSE_val;
		END3 = clock();
		//printf("MSE 執行所花費時間:%f\n", double(clock() / CLOCKS_PER_SEC));
		//printf("MSE 進行運算所花費時間:%f\n\n", (END3 - START3) / CLOCKS_PER_SEC);
		//fout << "Total time:" << double(clock() / CLOCKS_PER_SEC) << endl;
		//fout << "MSE:" << MSE_val << endl;
		printf("MSE:%f\n", MSE_val);

		END4 = clock();
		//fout << "Total time:" << (END4 - START4) / CLOCKS_PER_SEC << endl;
		g_total_time += (END4 - START4) / CLOCKS_PER_SEC;
		//printf("ans:%f,%f,%f\n", g_total_SRCC, g_total_MSE, g_total_time);
		return 0;
	}

	else if (sort_info.average_keyPoints > 600) {
		return 1;
	}
	else if (sort_info.average_keyPoints < 500) {
		frames_keyPoint.clear();
		frames_desp.clear();
		delete[] sort_info.frames_good_matches_pair;
		return 2;
	}

}

void FrameProcess::run(char folder[], int i, char name[]) {
	double START, END;
	START = clock();
	double ratio = 4.0f;
	int HessianThreshold = 1;
	int times = 0;
	uint speed_up = 4;
	int Hessian_val[20] = { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 , 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000 };
	int Hessian_val_s[10] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
	bool enforce = false;
	char image_name[300];
	int do_find = 0;
	while (true) {
		printf("Try => ratio = %.2f, HessianThreshold = %d\n", ratio, HessianThreshold);
		FrameProcess FP_temp;
		if (do_find >= 1) {
			first = false;
		}
		int result = FP_temp.reconstrutFrame(folder, ratio, HessianThreshold, speed_up, enforce, i, do_median, temp_image_arr, first, what_noise, name);
		
		if (FP_temp.do_median) {
			do_median = true;
		}

		if (result == 0) {
			if (speed_up != 1) {
				speed_up = 1;
				enforce = true;
			}
			else {
				END = clock();
				g_total_SRCC += FP_temp.g_total_SRCC;
				g_total_MSE += FP_temp.g_total_MSE;
				g_total_time += (END - START) / CLOCKS_PER_SEC;
				g_total_sort_time += FP_temp.g_total_sort_time;
				g_total_pre_time += (FP_temp.end_pre_time - START) / CLOCKS_PER_SEC;
				g_total_match_time += FP_temp.g_total_match_time;
				sprintf_s(image_name, "%s%s", name, "_result.txt"); // output result to name_result.txt
				ofstream fout(image_name);
				fout << folder << endl;
				fout << "HessianThreshold:" << HessianThreshold << endl;
				fout << "ratio:" << ratio << endl;
				if (what_noise == 0) {
					fout << "no noise" << endl;
				}
				else if (what_noise == 1) {
					fout << "do Curvature filter" << endl;
				}
				else if (what_noise == 2) {
					fout << "do median filter" << endl;
				}
				fout << "SRCC:" << FP_temp.g_total_SRCC << endl;
				fout << "MSE:" << FP_temp.g_total_MSE << endl;
				fout << "Prepocess time:" << (FP_temp.end_pre_time - START) / CLOCKS_PER_SEC << endl;
				fout << "Match time:" << FP_temp.g_total_match_time << endl;
				fout << "Sort time:" << FP_temp.g_total_sort_time << endl;
				fout << "Total time:" << (END - START) / CLOCKS_PER_SEC << endl;
				do_median = false;
				first = true;
				what_noise = 0;
				break;
			}
		}
		else if (result == 1) {
			HessianThreshold = Hessian_val[times];
			times += 1;
		}
		else if (result == 2) {
			if (times == 0) {
				ratio -= 1.0f;
			}
			else {
				HessianThreshold -= 3;
				if (HessianThreshold == 1 && ratio == 2) {
					enforce = true;
					printf("Can't find the parameter, enforce run\n");
				}
				else if (HessianThreshold == 1) {
					ratio -= 1.0f;
					times = 0;
				}
			}

		}
		if (times > 20) {
			break;
		}
		do_find += 1;
	}

}
double FrameProcess::SRCC(vector<uint16_t> groud_truth, vector<uint16_t> our_output) {

	double di = 0.0f;
	double di_inv = 0.0f;
	double ans = 0.0f;
	//正向計算
	for (int i = 0; i < groud_truth.size(); i++) {
		di += pow(i + 1 - our_output[i], 2);
	}
	//反向計算
	reverse(our_output.begin(), our_output.end());//只在這個範圍裡反轉，function結束後就會回復原來排序。
	for (int i = 0; i < groud_truth.size(); i++) {
		di_inv += pow(i + 1 - our_output[i], 2);
	}
	double div = (groud_truth.size() * (pow(groud_truth.size(), 2) - 1));
	double SRCC = abs(1.0f - (6 * di) / div);
	double SRCC_inv = abs(1.0f - (6 * di_inv) / div);
;
	if (di < di_inv) {
		ans = SRCC;
	}
	else {
		ans = SRCC_inv;
		inv = true;

	}
	printf("\n");
	printf("SRCC:%f", ans);
	return(ans);
}
double FrameProcess::MSE(vector<cv::Mat> groud_truth, vector<uint16_t> our_output_reorder) {
	double se_val_b = 0.0f;
	double se_val_g = 0.0f;
	double se_val_r = 0.0f;
	double mse_val = 0.0f;

	if (!inv) {
		printf(", 正向\n");
	}
	else {
		printf(", 反向\n");
		reverse(our_output_reorder.begin(), our_output_reorder.end());//reverse
	}

#pragma omp parallel for
	for (int i = 0; i < groud_truth.size(); i++) {
		for (int r = 0; r < groud_truth[i].rows; r++) {
			for (int c = 0; c < groud_truth[i].cols; c++) {
				se_val_b += pow(groud_truth[i].at<cv::Vec3b>(r, c)[0] - groud_truth[our_output_reorder[i] - 1].at<cv::Vec3b>(r, c)[0], 2);
				se_val_g += pow(groud_truth[i].at<cv::Vec3b>(r, c)[1] - groud_truth[our_output_reorder[i] - 1].at<cv::Vec3b>(r, c)[1], 2);
				se_val_r += pow(groud_truth[i].at<cv::Vec3b>(r, c)[2] - groud_truth[our_output_reorder[i] - 1].at<cv::Vec3b>(r, c)[2], 2);
			}
		}

	}
	mse_val = (se_val_b + se_val_g + se_val_r) / double((3.0f * groud_truth[0].total()));
	mse_val /= groud_truth.size();
	return mse_val;
}

//https://github.com/YuanhaoGong/CurvatureFilter
//=====================曲率濾波
int ItNum = 10;
int Type = 2;
float lambda = 1.0f;
float DataFitOrder = 1.0f;
//If use these filters to solve a complex data fitting term, define the data fitting as the blackbox function
float BlackBox(int row, int col, Mat& U, Mat& img_orig, float& d) {

	//this is an example of adaptive norm
	float diff = fabs(U.at<float>(row, col) + d - img_orig.at<float>(row, col));
	float order = 2 - (fabs(U.at<float>(row + 1, col) - U.at<float>(row, col)) + fabs(U.at<float>(row, col + 1) - U.at<float>(row, col)));
	return pow(diff, order);
}
void CurvatureFilter(int argc, const char* Filter_Type, int number, float for_lambda, float for_DataFitOrder, cv::Mat img) {
	CF DualMesh;
	if ((argc < 4) || (argc > 6))
	{
		cout << endl;
		cout << " -------------------- Curvature Filter ------------------------- " << endl;
		cout << " Please cite Yuanhao's PhD thesis and related papers. Thank you! " << endl;
		cout << " --------------------------------------------------------------- \n\n";
		cout << "usage: main imageName filterType Iterations.\nFor example: ./cf lena.bmp m 30\n";
		cout << "             or              " << endl;
		cout << "usage: main imageName filterType MaxItNum lambda DataFitOrder.\nFor example: ./cf lena.bmp m 30 1.2 1.5\n";
		cout << "************************************************\n";
		cout << "Possible Filter Type: t (Total Variation) \n";
		cout << "                      m (Mean Curvature) \n";
		cout << "                      d (Difference Curvature, not ready) \n";
		cout << "                      g (Gaussian Curvature) \n";
		cout << "                      b (Bernstein Filter) \n";
	}

	//DualMesh.read(argv[1]);
	DualMesh.read(img);

	const char* filterType = Filter_Type;
	switch (*filterType)
	{
	case 't':
	{ Type = 0; break; }
	case 'm':
	{ Type = 1; break; }
	case 'g':
	{ Type = 2; break; }
	case 'd':
	{ Type = 3; break; }
	case 'b':
	{ Type = 4; break; }
	default:
	{ cout << "Filter Type is NOT correct." << endl; break; }
	}

	ItNum = number;
	double mytime;

	//====================================just smooth the image by the filter
	if (argc == 4)
	{
		DualMesh.Filter(Type, mytime, ItNum);
		//cout << "runtime is " << mytime << " milliseconds." << endl;


		/*DualMesh.read(img);
		DualMesh.FilterNoSplit(Type, mytime, ItNum);
		cout << "runtime (noSplit) is " << mytime << " milliseconds." << endl;*/

	}

	//=====================================solve a variational model (data fitting term is blackbox)
	if (argc == 5)
	{
		lambda = for_lambda;

		DualMesh.read(img);
		DualMesh.BlackBoxSolver(Type, mytime, ItNum, lambda, BlackBox);
		//cout << "runtime is " << mytime << " milliseconds." << endl;

	}

	//solve a variational model
	if (argc == 6)
	{
		lambda = for_lambda;
		DataFitOrder = for_DataFitOrder;
		//filter solver for the variational models
		DualMesh.read(img);
		DualMesh.Solver(Type, mytime, ItNum, lambda, DataFitOrder);
		//cout << "runtime is " << mytime << " milliseconds." << endl;

	}
	cv::Mat tmp_save;


	tmp_save = DualMesh.write();
	//cout << tmp_save << endl;
	//waitKey(0);

	tmp_save.copyTo(img);

}
void do_CF(vector<cv::Mat> images, int argc, const char* Filter_Type, int number, float for_lambda, float for_DataFitOrder) {

	for (auto& img : images) {
		Mat noise_img = img.clone();
		Mat channel[3];
		split(noise_img, channel);

		CurvatureFilter(argc, Filter_Type, number, for_lambda, for_DataFitOrder, channel[0]);
		CurvatureFilter(argc, Filter_Type, number, for_lambda, for_DataFitOrder, channel[1]);
		CurvatureFilter(argc, Filter_Type, number, for_lambda, for_DataFitOrder, channel[2]);

		cv::merge(channel, 3, noise_img);
		noise_img.copyTo(img);
	}
}
