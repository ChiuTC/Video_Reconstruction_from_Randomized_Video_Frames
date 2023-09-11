#include "QmoHeader.h"
#include "haze.h"


int main() {
	//////////////////實作程式//////////////////////////
	// 只需要 QmoHeader.h 和 YCbCr_Y.cpp 
	// public.cpp是讀檔案的程式
	// 
	//vector<cv::Mat> out_img;					
	//vector<cv::Mat> out_img2;
	//YCbCr_Y YCbCr_Y;
	// 
	//YCbCr_Y.power_law(images, out_img);
	//YCbCr_Y.Correction(out_img, out_img2);
	// 
	////////////////////////////////////////////////////

	while (1) {

		int priceA;
		printf("請輸入題號：");
		cin >> priceA;


		if (priceA == 1) {

			string inputimg_path = getpath();
			vector<cv::Mat> images;
			load_data(inputimg_path, images);

			int k = 0;

			clock_t t1, t2;
			t1 = clock();

			vector<cv::Mat> out_img;
			vector<cv::Mat> out_img2;
			YCbCr_Y YCbCr_Y;
			YCbCr_Y.power_law(images, out_img);

			YCbCr_Y.Correction(out_img, out_img2);

			t2 = clock();
			printf("TIME :%lf\n", (t2 - t1) / (double)(CLOCKS_PER_SEC));

			for (auto& img : out_img2) {
				k += 1;
				char file_name[100] = "Desert_out_C";
				if (k < 10) {
					char tmpword[100];
					sprintf_s(tmpword, "%s%s%s%d%s", "./",file_name,"/00", k, ".jpg");

					imwrite(tmpword, img);
				}
				else if (k < 100) {
					char tmpword[100];
					sprintf_s(tmpword, "%s%s%s%d%s", "./", file_name, "/0", k, ".jpg");

					imwrite(tmpword, img);
				}
				else if (k < 1000) {
					char tmpword[100];
					sprintf_s(tmpword, "%s%s%s%d%s", "./", file_name, "/", k, ".jpg");

					imwrite(tmpword, img);
				}
				
			}
		}
		/*else if (priceA == 2) {
			test();
		}
		else if (priceA == 3) {
			string inputimg_path = getpath();
			vector<cv::Mat> images;
			load_data(inputimg_path, images);

			printf("影片輸出中...\n");
			cv::VideoWriter video("test004.mp4", cv::CAP_OPENCV_MJPEG, 25.0, Size(640, 480));

			for (auto& img : images) {
				cv::resize(img, img, Size(640, 480));
				video.write(img);
			}
			printf("影片輸出完成!!\n");

		}
		else if (priceA == 4) {
			PUBG PUBG;
			PUBG.HSL_Median();
		}
		else if (priceA == 5) {
			cv::Mat load = cv::imread("./RushPixar_out/007.jpg");
			cv::Mat result;
			Haze haze;
			haze.dehaze(load, result);
			cv::imshow("src", load);
			cv::imshow("result", result);
			imwrite("result007.jpg", result);
			cv::waitKey(0);
		}*/
		else if (priceA == 0) {
			return 0;
		}
		else {
			printf("\n輸入錯誤!!\n\n");
		}
	}

}

