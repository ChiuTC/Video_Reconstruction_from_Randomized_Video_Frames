#include "QmoHeader.h"


string getpath() {
	string inputimg_path;

	TCHAR szPathName[MAX_PATH] = { 0 };
	BROWSEINFO bInfo = { 0 };
	bInfo.hwndOwner = GetForegroundWindow(); 
	bInfo.lpszTitle = TEXT("選擇資料夾");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI 
		| BIF_UAHINT ;
	//來自於 https://blog.csdn.net/qq_40969467/article/details/112609778
	LPITEMIDLIST lpDlist;
	lpDlist = SHBrowseForFolder(&bInfo);
	if (lpDlist != NULL) 
	{
		SHGetPathFromIDList(lpDlist, szPathName);
		cout << szPathName << endl;
		std::basic_string<TCHAR> strName = szPathName; 
		inputimg_path = strName;
	}
	else { printf("取消"); }

	return inputimg_path;
}
void load_data(string& path, vector<Mat>& images) {
	//https://yypw.wordpress.com/2020/06/21/%E9%9B%BB%E8%85%A6%E8%A6%96%E8%A6%BA-%E4%BD%BF%E7%94%A8-opencv-%E8%AE%80%E5%8F%96%E8%B3%87%E6%96%99%E5%A4%BE%E5%85%A7%E5%85%A8%E9%83%A8%E5%9C%96%E7%89%87/
	vector<cv::String> file_names;
	cv::glob(path, file_names);
	size_t s = file_names.size();
	images.resize(s);
	cout << "Start loading" << endl;
	for (size_t i = 0; i < s; ++i) {
		images[i] = imread(file_names[i]);
	}
	cout << "End loading" << endl;
}


