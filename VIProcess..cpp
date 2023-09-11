#include "VIProcess.h"


void VIProcess::Video_To_Image(string file_name) {
	printf("*----Video To Image----*\n");
	cv::VideoCapture capture(file_name);
	if (!capture.isOpened()) {
		printf("open video error\n");
	}
	int frame_width = int(capture.get(cv::CAP_PROP_FRAME_WIDTH));
	int frame_height = int(capture.get(cv::CAP_PROP_FRAME_HEIGHT));
	float frame_fps = capture.get(cv::CAP_PROP_FPS);
	int frame_num = capture.get(cv::CAP_PROP_FRAME_COUNT);
	printf("frame_width = %d\n", frame_width);
	printf("frame_height = %d\n", frame_height);
	printf("frame_fps = %f\n", frame_fps);

	int num = 0;
	cv::Mat img;
	string img_name;
	char image_name[30];
	namedWindow("MyVideo", cv::WINDOW_AUTOSIZE);
	while (true) {
		cv::Mat frame;
		bool bSuccess = capture.read(frame);
		if (!bSuccess) {
			printf("can't read frame from video");
			break;
		}
		//imshow("MyVideo", frame);
		sprintf_s(image_name, "%s%d%s", "./test2_frame/", ++num, ".png");
		img_name = image_name;;
		imwrite(img_name, frame);
		if (cv::waitKey(30) == 27 || num == frame_num) {
			printf("press ESC");
			break;
		}//27¬°ESCªºASCII½X
	}
	capture.release();
}
void VIProcess::Image_To_Video(float frame_fps, int input_frame_num) {
	printf("*----Imgae To Video----*\n");
	char image_name[50];
	string s_image_name;
	cv::VideoWriter writer;
	int isColor = 1;
	sprintf_s(image_name, "%s%s", "./dataset/typing/Ground truth/", "001.png");
	s_image_name = image_name;
	cv::Mat test_img = cv::imread(s_image_name);

	int frame_width = int(test_img.cols);
	int frame_height = int(test_img.rows);

	string video_name = "out.avi";
	writer = cv::VideoWriter(video_name, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), frame_fps, cv::Size(frame_width, frame_height), isColor);
	printf("frame_width = %d\n", frame_width);
	printf("frame_height = %d\n", frame_height);
	printf("frame_fps = %f\n", frame_fps);

	namedWindow("image to video", cv::WINDOW_AUTOSIZE);
	int i = 0;
	int H = 0, T = 0, U = 0;
	cv::Mat img;
	while (i < input_frame_num) {
		++i;
		++U;
		if (U == 10) {
			U = 0;
			T += 1;
		}
		if (T == 10) {
			T = 0;
			H += 1;
		}
		sprintf_s(image_name, "%s%d%d%d%s", "./dataset/typing/Ground truth/", H, T, U, ".png");
		
		s_image_name = image_name;
		//printf("%s\n", image_name);
		img = cv::imread(s_image_name);

		if (!img.data) {
			printf("can't load frame file");
			break;
		}
		//imshow("image to video", img);
		writer.write(img);

		if (cv::waitKey(30) == 27 || i == input_frame_num) {
			printf("press ESC");
			break;
		}	
	}
}