#pragma once

#include"PF_BufferMgr.h"

//管理空闲页面
class Page_Mgr {
public:

	static Page_Mgr page_mgr;
	Page_Mgr();

	//将页面信息写回外存
	int Write_page();

	//分配新的页面
	int Allocate_page_to_file(string dir);

	//释放空闲页面
	int Recover_page(string dir, int Page_id);

	//下一个分配的page-id
	int allocate_page_id;
	vector<int> free_page_id_list;
};

