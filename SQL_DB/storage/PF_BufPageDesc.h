#pragma once
#include"../global.h"

//ÿһ��������
class PF_BufPageDesc {
public:
	PF_BufPageDesc();

	//�������洢page��dir
	string dir;
	int    pageNum;

	char* pData;
	//����LRU
	int time;

};