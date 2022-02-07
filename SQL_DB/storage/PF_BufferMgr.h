#pragma once
#include"PF_BufPageDesc.h"


//����������
class PF_BufferMgr {
public:
	static PF_BufferMgr pf_buffermgr;
	//��ȡpage��������
	int Read_page_to_buffer(string dir, int pageNum);

	//д�����
	int Write_buffer_to_page(int buffer_id);

	//���л�����д�����
	int Write_all_buffer();


	//�ж�page�Ƿ��ڻ�����
	int If_page_in_buffer(int pageNum);

	//LRU�滻
	int LRU_get_buffer_id(string insert_dir, int insert_pageNum);

public:
	//��������С
	int occupy_buffer_size;

	//����������
	PF_BufPageDesc buffer_list[BUFFER_NUM];

	//page���ڻ�����
	unordered_map<int, int> Page_to_buffer_mp;
};

