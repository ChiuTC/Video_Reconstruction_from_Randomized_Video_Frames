#undef UNICODE

#include"windows.h"
#include<shlobj.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include <string>
#include <vector>
#include<math.h>
#include <random>

#include<opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>   // for imshow
#include <opencv2/imgcodecs.hpp> // for imread

#include <fstream>
#include <assert.h>
#include <time.h>
#include <cmath>
//only needed by studying curvature statistics
#if defined(statistics) 
#include <dirent.h>
#endif


using namespace cv;
using namespace std;
//<-----------定義輸入資料夾


class folder {
private:
	
public:
	string inputimg_path;
	char name[300];
	char path[300];
	void getpath();
};
//void getpath(); void load_data();
//void getpath() {
//	string inputimg_path;
//	char path[100];
//	int size = 0;
//	TCHAR szPathName[MAX_PATH] = { 0 };
//	BROWSEINFO bInfo = { 0 };
//	bInfo.hwndOwner = GetForegroundWindow(); // 父窗口;
//	bInfo.lpszTitle = TEXT("選擇資料夾");
//	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI /*包含一个编辑框 用户可以手动填写路径 对话框可以调整大小之类的..;*/
//		| BIF_UAHINT /*带TIPS提示*/ /*| BIF_NONEWFOLDERBUTTON 不带新建文件夹按钮*/;
//	//來自於 https://blog.csdn.net/qq_40969467/article/details/112609778
//	LPITEMIDLIST lpDlist;
//	lpDlist = SHBrowseForFolder(&bInfo);
//	
//	if (lpDlist != NULL) // 单击了确定按钮;
//	{
//		SHGetPathFromIDList(lpDlist, szPathName);
//		cout << szPathName << endl;
//		std::basic_string<TCHAR> strName = szPathName; //使用的方法來自https://www.codeproject.com/Questions/99948/converting-TCHAR-to-string-in-c
//		inputimg_path = strName;
//		for (int i = 0; i < 100; i++) {
//			path[i] = inputimg_path[i];
//			if (inputimg_path[i] == '\\') {
//				path[i] = '/';
//			}
//		}
//		//printf("path:%s\n", path);
//	}
//	else { printf("取消"); }
//}
////void load_data(string& path, vector<Mat>& images) {
//////https://yypw.wordpress.com/2020/06/21/%E9%9B%BB%E8%85%A6%E8%A6%96%E8%A6%BA-%E4%BD%BF%E7%94%A8-opencv-%E8%AE%80%E5%8F%96%E8%B3%87%E6%96%99%E5%A4%BE%E5%85%A7%E5%85%A8%E9%83%A8%E5%9C%96%E7%89%87/
////	vector<cv::String> file_names;
////	cv::glob(path, file_names);
////	size_t s = file_names.size();
////	images.resize(s);
////	cout << "Start loading" << endl;
////	for (size_t i = 0; i < s; ++i) {
////		images[i] = imread(file_names[i]);
////	}
////	cout << "End loading" << endl;
////}
//

