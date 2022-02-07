#pragma once
#include"../global.h"

//每一个缓冲区
class PF_BufPageDesc {
public:
	PF_BufPageDesc();

	//缓冲区存储page的dir
	string dir;
	int    pageNum;

	char* pData;
	//用来LRU
	int time;

};