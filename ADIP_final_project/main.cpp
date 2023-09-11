#include <iostream>
#include <cstdio>
#include <opencv2/core/utils/logger.hpp>
#include "VIProcess.h"
#include "FrameProcess.h"
#include "folder.h"

using namespace std;
//using namespace cv;



VIProcess VIP;
FrameProcess* FP;
folder fd;
int main() {
	
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
	//VIP.Video_To_Image("test2.mp4");
	//VIP.Image_To_Video(48.0f, 720);

	vector<string> folders_n;
	folders_n.push_back("ADIP_Final_project_dataset/beach");
	folders_n.push_back("ADIP_Final_project_dataset/Boat_style");
	folders_n.push_back("ADIP_Final_project_dataset/candle_style");
	folders_n.push_back("ADIP_Final_project_dataset/CCTV");
	folders_n.push_back("ADIP_Final_project_dataset/coastline");
	folders_n.push_back("ADIP_Final_project_dataset/Desert");
	folders_n.push_back("ADIP_Final_project_dataset/DMC");
	folders_n.push_back("ADIP_Final_project_dataset/flyout");
	folders_n.push_back("ADIP_Final_project_dataset/helltaker");
	folders_n.push_back("ADIP_Final_project_dataset/PUBG");
	folders_n.push_back("ADIP_Final_project_dataset/RushPixar");
	folders_n.push_back("ADIP_Final_project_dataset/School");
	folders_n.push_back("ADIP_Final_project_dataset/soccer_style");
	folders_n.push_back("ADIP_Final_project_dataset/TKUC");
	folders_n.push_back("ADIP_Final_project_dataset/typing");
	

	/*double do_num = 0.0f;
	for (int i = 0; i < folders_n.size() ; i++) {
		char* cstr = new char[folders_n[i].length() + 1];
		strcpy_s(cstr, folders_n[i].length() + 1, folders_n[i].c_str());
		FP.run(cstr, i);
		do_num += 1;
	}
	printf("ans_avg:%f,%f,%f\n", FP.g_total_SRCC / do_num, FP.g_total_MSE / do_num, FP.g_total_time / do_num);*/
	//ofstream fout("ans.txt");
	//fout << "avg_SRCC:" << FP.g_total_SRCC / do_num << endl;
	//fout << "avg_MSE:" << FP.g_total_MSE / do_num << endl;
	//fout << "avg_time:" << FP.g_total_time / do_num << endl;
	fd.getpath();
	char* cstr = new char[fd.inputimg_path.length() + 1];
	strcpy_s(cstr, fd.inputimg_path.length() + 1, fd.inputimg_path.c_str());
	
	FP->run(fd.path, 0, fd.name);

	int input = 0;
	printf("press -1 to exit\n");
	scanf_s("%d", &input);
	switch (input) {
	case -1:
		break;
	}
}
