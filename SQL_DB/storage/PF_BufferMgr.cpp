#include "PF_BufferMgr.h"


//读取page到缓冲区
int PF_BufferMgr::Read_page_to_buffer(string dir, int pageNum) {
	if (If_page_in_buffer(pageNum) != -1) {
		return If_page_in_buffer(pageNum);
	}

	int buffer_id;
	if (occupy_buffer_size < BUFFER_NUM) {
		buffer_id = occupy_buffer_size;
		occupy_buffer_size++;

		buffer_list[buffer_id].time = 0;
		buffer_list[buffer_id].pageNum = pageNum;
		buffer_list[buffer_id].dir = dir;

		Page_to_buffer_mp.insert(pair<int, int>(buffer_list[buffer_id].pageNum, buffer_id));
	}
	else {
		buffer_id = LRU_get_buffer_id(dir, pageNum);
	}

	FILE* fp;
	string filename = dir + "/" + to_string(pageNum);
	if ((fp = fopen(filename.c_str(), "ab+")) == NULL) {
		printf("Fail to open file!\n");
		exit(0);
	}
	fseek(fp, 0, 2);
	int num = ftell(fp);
	fseek(fp, 0, 0);
	fread(buffer_list[buffer_id].pData, sizeof(char), num, fp);
	fclose(fp);

	return buffer_id;
}

//写回外存
int PF_BufferMgr::Write_buffer_to_page(int buffer_id) {

	FILE* fp;
	string dir = buffer_list[buffer_id].dir;  int pageNum = buffer_list[buffer_id].pageNum;
	string filename = dir + "/" + to_string(pageNum);
	if ((fp = fopen(filename.c_str(), "wb+")) == NULL) {
		printf("Fail to open file!\n");
		exit(0);
	}
	int* p = (int*)(buffer_list[buffer_id].pData + 4);

	fwrite(buffer_list[buffer_id].pData, sizeof(char), *p, fp);
	fclose(fp);

	return 0;
}

//所有缓冲区写回外存
int PF_BufferMgr::Write_all_buffer() {
	for (int i = 0; i < occupy_buffer_size; i++) {
		Write_buffer_to_page(i);
	}
	return 0;
}


//判断page是否在缓冲区
int PF_BufferMgr::If_page_in_buffer(int pageNum) {
	int buffer_id = -1;
	try {
		buffer_id = Page_to_buffer_mp.at(pageNum);
	}
	catch (const std::exception&) {
		return -1;
	}
	return Page_to_buffer_mp.at(pageNum);
}

//LRU替换
int PF_BufferMgr::LRU_get_buffer_id(string insert_dir, int insert_pageNum) {
	int index = 0;
	for (int i = 0; i < BUFFER_NUM; i++) {
		if (buffer_list[index].time < buffer_list[i].time) {
			index = i;
		}
	}
	for (int i = 0; i < BUFFER_NUM; i++) {
		buffer_list[i].time++;
	}

	Write_buffer_to_page(index);
	Page_to_buffer_mp.at(buffer_list[index].pageNum) = -1;

	buffer_list[index].time = 0;
	buffer_list[index].pageNum = insert_pageNum;
	buffer_list[index].dir = insert_dir;
	//

	try {
		Page_to_buffer_mp.at(buffer_list[index].pageNum) = 1;
	}
	catch (const std::exception&) {
		Page_to_buffer_mp.insert(pair<int, int>(buffer_list[index].pageNum, 1));
	}

	return index;

}

PF_BufferMgr PF_BufferMgr::pf_buffermgr;