#include "folder.h"
//void folder::getpath() {
//	int size = 0;
//	TCHAR szPathName[MAX_PATH] = { 0 };
//}

void folder::getpath() {

	int size = 0;
	TCHAR szPathName[MAX_PATH] = { 0 };
	BROWSEINFO bInfo = { 0 };
	bInfo.hwndOwner = GetForegroundWindow(); // 父窗口;
	bInfo.lpszTitle = TEXT("選擇資料夾");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI 
		| BIF_UAHINT;
	//來自於 https://blog.csdn.net/qq_40969467/article/details/112609778
	LPITEMIDLIST lpDlist;
	lpDlist = SHBrowseForFolder(&bInfo);

	if (lpDlist != NULL) 
	{
		SHGetPathFromIDList(lpDlist, szPathName);
		cout << szPathName << endl;
		std::basic_string<TCHAR> strName = szPathName; //使用的方法來自https://www.codeproject.com/Questions/99948/converting-TCHAR-to-string-in-c
		inputimg_path = strName;
		for (int i = 0; i < inputimg_path.length(); i++) {
			path[i] = inputimg_path[i];
			if (inputimg_path[i] == '\\') {
				path[i] = '/';
			}
		}
		int cur_i = 0;
		for (int i = inputimg_path.length() - 1; i > -1; i--) {
			if (inputimg_path[i] == '\\') {
				cur_i = i + 1;
				break;
			}
		}
		for (int i = cur_i; i < inputimg_path.length(); i++) {
			name[i - cur_i] = inputimg_path[i];
		}
		//printf(name);
		//printf("path:%s\n", path);
	}
	else { printf("取消"); }
}