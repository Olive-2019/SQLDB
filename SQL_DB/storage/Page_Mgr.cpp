#include "Page_Mgr.h"


Page_Mgr::Page_Mgr() {
	FILE* fp;
	string filename = "sys/0";
	if ((fp = fopen(filename.c_str(), "ab+")) == NULL) {
		printf("Fail to open file!\n");
		exit(0);
	}
	int a, b;
	fread(&a, sizeof(int), 1, fp);
	fread(&b, sizeof(int), 1, fp);
	allocate_page_id = a;
	for (int i = 0; i < b; i++) {
		fread(&a, sizeof(int), 1, fp);
		free_page_id_list.push_back(a);
	}
	fclose(fp);
}

//将页面信息写回外存
int Page_Mgr::Write_page() {
	FILE* fp;
	string filename = "sys/0";
	if ((fp = fopen(filename.c_str(), "wb+")) == NULL) {
		printf("Fail to open file!\n");
		exit(0);
	}
	int a = allocate_page_id;
	int b = free_page_id_list.size();
	fwrite(&a, sizeof(int), 1, fp);
	fwrite(&b, sizeof(int), 1, fp);

	for (int i = 0; i < b; i++) {
		fwrite(&free_page_id_list[i], sizeof(int), 1, fp);
	}
	fclose(fp);
}

//分配新的页面
int Page_Mgr::Allocate_page_to_file(string dir) {
	int res;
	if (free_page_id_list.empty() == false) {
		int res = free_page_id_list[free_page_id_list.size() - 1];
		free_page_id_list.pop_back();
		return res;
	}
	allocate_page_id++;
	res = allocate_page_id - 1;

	FILE* fp;
	string filename = dir + "\\" + to_string(res);
	if ((fp = fopen(filename.c_str(), "ab+")) == NULL) {
		printf("Fail to open file!\n");
		exit(0);
	}
	int a = -1, b = 8;
	fwrite(&a, sizeof(int), 1, fp);
	fwrite(&b, sizeof(int), 1, fp);
	fclose(fp);
	return res;
}

//释放空闲页面
int Page_Mgr::Recover_page(string dir, int Page_id) {
	free_page_id_list.push_back(Page_id);
	string filename = dir + to_string(Page_id);
	remove(filename.c_str());
	return 0;
}

Page_Mgr Page_Mgr::page_mgr;