#pragma once

#include"PF_BufferMgr.h"

//�������ҳ��
class Page_Mgr {
public:

	static Page_Mgr page_mgr;
	Page_Mgr();

	//��ҳ����Ϣд�����
	int Write_page();

	//�����µ�ҳ��
	int Allocate_page_to_file(string dir);

	//�ͷſ���ҳ��
	int Recover_page(string dir, int Page_id);

	//��һ�������page-id
	int allocate_page_id;
	vector<int> free_page_id_list;
};

