#include "YCbCr_Y.h"


void YCbCr_Y::power_law(vector<cv::Mat> input_img) {
	for (int i = 0; i < input_img.size(); i++) {
		//cv::Mat imaIN_2 = input_img[i];

		cv::Mat imgYCrCb;
		cv::Mat imgYCrCb_OUT;
		cv::Mat imgOUT;

		cvtColor(input_img[i], imgYCrCb, cv::COLOR_BGR2YCrCb);

		vector<cv::Mat> mv(3);
		split(imgYCrCb, mv); //type CV_8U

		cv::Scalar tempval = cv::mean(mv[0]);
		float matMean = tempval.val[0];

		if (matMean < 40) {
			for (size_t nrow = 0; nrow < mv[0].rows; nrow++)
			{
				for (size_t ncol = 0; ncol < mv[0].cols; ncol++)
				{
					std::min(mv[0].at<uchar>(nrow, ncol) += std::min(double(40 - matMean), 10.0), uchar(255));
				}
			}
		}

		if (matMean > 190) {
			for (size_t nrow = 0; nrow < mv[0].rows; nrow++)
			{
				for (size_t ncol = 0; ncol < mv[0].cols; ncol++)
				{
					std::max(mv[0].at<uchar>(nrow, ncol) -= std::min(double(matMean - 190), 10.0), uchar(0));
				}
			}
		}

		double r;
		r = (matMean + 60) / (128 + 60);

		mv[0].convertTo(mv[0], CV_32F, 1.0 / 255);
		pow(mv[0], r, mv[0]);
		mv[0].convertTo(mv[0], CV_8U, 255 / 1.0);

		merge(mv, imgYCrCb_OUT);

		cvtColor(imgYCrCb_OUT, imgYCrCb_OUT, cv::COLOR_YCrCb2BGR);
		
		output_img_p.push_back(imgYCrCb_OUT);
		//imgYCrCb_OUT.copyTo(input_img[i]);
	}
	printf("power law complete\n");
}

void YCbCr_Y::Correction(vector<cv::Mat> input_img) {

	double mean_avg = 0;
	vector<cv::Mat> input_img_YCbCr;

	for (int i = 0; i < input_img.size(); i++) {
		cv::Mat imgYCrCb;
		cvtColor(input_img[i], imgYCrCb, cv::COLOR_BGR2YCrCb);
		input_img_YCbCr.push_back(imgYCrCb);

		vector<cv::Mat> mv(3);
		split(imgYCrCb, mv); //type CV_8U

		cv::Scalar tempval = cv::mean(mv[0]);
		float matMean = tempval.val[0];

		mean_avg += matMean;
	}

	mean_avg = mean_avg / double(input_img.size());
	printf("mean_avg : %f\n", mean_avg);

	for (auto& img: input_img_YCbCr) {

		cv::Mat imgYCrCb_OUT;

		vector<cv::Mat> mv(3);
		split(img, mv); //type CV_8U

		cv::Scalar tempval = cv::mean(mv[0]);
		float matMean = tempval.val[0];

		for (size_t nrow = 0; nrow < mv[0].rows; nrow++)
		{
			for (size_t ncol = 0; ncol < mv[0].cols; ncol++)
			{
				int maxmin = mv[0].at<uchar>(nrow, ncol) + mean_avg - matMean;
				if (maxmin > 255) {
					mv[0].at<uchar>(nrow, ncol) = 255;
				}
				else if (maxmin < 0) {
					mv[0].at<uchar>(nrow, ncol) = 0;
				}
				else {
					mv[0].at<uchar>(nrow, ncol) = maxmin;
				}
			}
		}

		merge(mv, imgYCrCb_OUT);

		cvtColor(imgYCrCb_OUT, imgYCrCb_OUT, cv::COLOR_YCrCb2BGR);
		
		output_img_c.push_back(imgYCrCb_OUT);
		//imgYCrCb_OUT.copyTo(input_img[i]);
		
	}
	
}


