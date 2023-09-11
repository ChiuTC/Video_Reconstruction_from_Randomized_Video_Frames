#pragma once
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
//using namespace cv;
class VIProcess {
private:
public:
	void Video_To_Image(string file_name);
	void Image_To_Video(float frame_fps, int input_frame_num);
};