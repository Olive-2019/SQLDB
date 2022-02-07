#pragma once
#include"PF_BufPageDesc.h"


//缓冲区管理
class PF_BufferMgr {
public:
	static PF_BufferMgr pf_buffermgr;
	//读取page到缓冲区
	int Read_page_to_buffer(string dir, int pageNum);

	//写回外存
	int Write_buffer_to_page(int buffer_id);

	//所有缓冲区写回外存
	int Write_all_buffer();


	//判断page是否在缓冲区
	int If_page_in_buffer(int pageNum);

	//LRU替换
	int LRU_get_buffer_id(string insert_dir, int insert_pageNum);

public:
	//缓冲区大小
	int occupy_buffer_size;

	//缓冲区集合
	PF_BufPageDesc buffer_list[BUFFER_NUM];

	//page所在缓冲区
	unordered_map<int, int> Page_to_buffer_mp;
};

