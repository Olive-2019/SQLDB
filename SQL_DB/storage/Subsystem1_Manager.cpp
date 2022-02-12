#include "Subsystem1_Manager.h"

//ɨ�����б�
void Subsystem1_Manager::Scan_rel() {
	int operate_page = 6;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			int* record_num = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* page_id_head = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			if (*exist == 1) {
				cout << string(db_name) << " " << string(rel_name) << " " << string(creator) << " " << *record_num << " " << *page_id_head << endl;
			}

		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
}

//ɨ����������
void Subsystem1_Manager::Scan_attribute() {
	int operate_page = 1;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Attribute", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* attr_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			int* attr_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* length = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* offset = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			/*���棺ͬ��


			int* distribution_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param1 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param2 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;*/

			char temp[1000];
			for (int i = 0; i < 100; i++) {
				temp[i] = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[scan_begin];
				scan_begin++;
			}
			Distribution* dis_p = (Distribution*)temp;



			int* num_of_change_records = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			if (*exist == 1) {
				cout << string(db_name) << " " << string(rel_name) << " " << string(attr_name) << " " << string(creator) << " ";
				if (*attr_type == 0) {
					cout << "INT";
				}
				else if (*attr_type == 1) {
					cout << "FLOAT";
				}
				else if (*attr_type == 2) {
					cout << "STRING";
				}
				cout << " " << *length << " " << *offset << " ";
				/*���棺ͬ��
				if (*distribution_type == 0) {
					cout << "NORMAL::" << *param1 << " " << *param2;
				}
				else {
					cout << "EVENLY::" << *param1 << " " << *param2;;
				}
				*/
				if (dis_p->type == 0) {
					cout << "NORMAL::" << " " << dis_p->normal_dis.mu << " " << dis_p->normal_dis.sigma;
				}
				else {
					cout << "EVENLY::" << " " << dis_p->evenly_dis.MIN << " " << dis_p->evenly_dis.MAX;
				}
				cout << " " << dis_p->type;
				cout << " num_of_change_records" << *num_of_change_records << endl;
			}

		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
}
//������������
vector<Attr_Info> Subsystem1_Manager::lookup_Attrs(string Rel_Name) {
	vector<Attr_Info> vec;
	int operate_page = 1;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Attribute", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* attr_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			int* attr_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* length = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* offset = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;


			/**����
			 ��ʼʱ����δ�洢��3��ֵ������ʹ�ýṹ�壨char����洢��
			int* distribution_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param1 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param2 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;*/

			char temp[1000];
			for (int i = 0; i < 100; i++) {
				temp[i] = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[scan_begin];
				scan_begin++;
			}
			Distribution* dis_p = (Distribution*)temp;


			int* num_of_change_records = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;

			if (string(db_name) == DBName && string(creator) == UserName && string(rel_name) == Rel_Name && *exist == 1) {
				Attr_Info attr_info;
				strcpy(attr_info.Attr_Name, attr_name); strcpy(attr_info.Creator, creator); strcpy(attr_info.DBName, db_name);
				strcpy(attr_info.Rel_Name, rel_name);
				attr_info.Length = *length; attr_info.Offset = *offset; attr_info.Num_of_Change_Records = *num_of_change_records;
				if (*attr_type == 0) {
					attr_info.type = AttrType::INT;
				}
				else if (*attr_type == 1) {
					attr_info.type = AttrType::FLOAT;
				}
				else if (*attr_type == 2) {
					attr_info.type = AttrType::STRING;
				}

				attr_info.distribution = *dis_p;
				/*
				����
				ͬ��
				if (*distribution_type == 0) {
					attr_info.distribution.type = Distribution_Type::NORMAL;
					attr_info.distribution.nor_eve.nor.mu = *param1;
					attr_info.distribution.nor_eve.nor.sigma = *param2;
				}
				else {
					attr_info.distribution.type = Distribution_Type::EVENLY;
					attr_info.distribution.nor_eve.eve.MIN = *param1;
					attr_info.distribution.nor_eve.eve.MAX = *param2;
				}
				*/

				vec.push_back(attr_info);
			}
			/*if (*exist == 1) {
				cout << string(db_name) << " " << string(rel_name) << " " << string(attr_name) << " " << string(creator) << " ";
				if (*attr_type == 0) {
					cout << "INT";
				}
				else if (*attr_type == 1) {
					cout << "FLOAT";
				}
				else if (*attr_type == 2) {
					cout << "STRING";
				}
				cout << " " << *length << " " << *offset << " ";
				if (*distribution_type == 0) {
					cout << "NORMAL::" << param1 << " " << param2;
				}
				else {
					cout << "EVENLY::" << param1 << " " << param2;;
				}
				cout << " " << *num_of_change_records << endl;
			}*/

		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
	return vec;

}


//�½����ݱ�
void Subsystem1_Manager::Create_Rel(string RelName, vector<Attr_Info> attrs) {
	cout << "create RelName==" << RelName << endl;
	Rel_Info rel_info;
	memcpy(rel_info.Rel_Name, RelName.c_str(), RelName.length() + 1);
	memcpy(rel_info.DBName, DBName.c_str(), DBName.length() + 1);
	memcpy(rel_info.Creator, UserName.c_str(), UserName.length() + 1);
	rel_info.Record_Num = 0;
	Add_rel(rel_info);
	Add_attribute(attrs);
	cout << "end create RelName==" << RelName << endl;
}



void Subsystem1_Manager::Add_rel(Rel_Info rel_info) {

	string com = "mkdir data\\" + string(rel_info.Creator) + "\\" + string(rel_info.DBName) + "\\" + string(rel_info.Rel_Name);
	system(com.c_str());
	string com2 = "data\\" + string(rel_info.Creator) + "\\" + string(rel_info.DBName) + "\\" + string(rel_info.Rel_Name);
	rel_info.PageID_Head = Page_Mgr::page_mgr.Allocate_page_to_file(com2.c_str());

	int operate_page = 6;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (begin >= Global_Paras::Block_Size) {
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;

		if (operate_page == -1) {
			operate_page = Page_Mgr::page_mgr.Allocate_page_to_file("sys/Relation");

			int* p = &operate_page;
			char* t_ch = (char*)p;
			for (int i = 0; i < 4; i++) {
				PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[i] = t_ch[i];
			}

		}
		buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
		pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
		begin = *pp;
	}

	//---------------------------------------------------------------------------------------
	int exist = 1;
	int* exist_p = &exist;
	char* exist_ch = (char*)exist_p;
	for (int i = 0; i < 4; i++) {
		PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = exist_ch[i];
		begin++;
	}


	int len = string(rel_info.DBName).length();
	for (int i = 0; i < len; i++) {
		PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = rel_info.DBName[i];
		begin++;
	}
	for (int i = 0; i < 20 - len; i++) {
		PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = '\0';
		begin++;
	}
	len = string(rel_info.Rel_Name).length();
	for (int i = 0; i < len; i++) {
		PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = rel_info.Rel_Name[i];
		begin++;
	}
	for (int i = 0; i < 20 - len; i++) {
		PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = '\0';
		begin++;
	}
	len = string(rel_info.Creator).length();
	for (int i = 0; i < len; i++) {
		PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = rel_info.Creator[i];
		begin++;
	}
	for (int i = 0; i < 20 - len; i++) {
		PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = '\0';
		begin++;
	}
	int* p = &rel_info.Record_Num;
	char* t_ch = new char[4];
	t_ch = (char*)p;
	for (int i = 0; i < 4; i++) {
		PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = t_ch[i];
		begin++;
	}
	p = &rel_info.PageID_Head;
	t_ch = (char*)p;
	for (int i = 0; i < 4; i++) {
		PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = t_ch[i];
		begin++;
	}

	p = &begin;
	t_ch = (char*)p;
	for (int i = 0; i < 4; i++) {
		PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[i + 4] = t_ch[i];
	}
	//PF_BufferMgr::pf_buffermgr.Write_buffer_to_page(buffer_id);


	//---------------------------------------------------------------------------------------------------------------

	//Add_attribute(rel_info.attr_list); ע��


}
void Subsystem1_Manager::Add_attribute(vector<Attr_Info> attr_list) {
	int size = attr_list.size();

	int operate_page = 1;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Attribute", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (begin >= Global_Paras::Block_Size) {
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;

		if (operate_page == -1) {
			operate_page = Page_Mgr::page_mgr.Allocate_page_to_file("sys/Attribute");

			int* p = &operate_page;
			char* t_ch = (char*)p;
			for (int i = 0; i < 4; i++) {
				PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[i] = t_ch[i];
			}

		}
		buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Attribute", operate_page);
		pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
		begin = *pp;
	}
	//--------------------------------------------------------------------------------------






	for (int j = 0; j < size; j++) {

		int exist = 1;
		int* exist_p = &exist;
		char* exist_ch = (char*)exist_p;
		for (int i = 0; i < 4; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = exist_ch[i];
			begin++;
		}

		int len = string(attr_list[j].DBName).length();
		for (int i = 0; i < len; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = attr_list[j].DBName[i];
			begin++;
		}
		for (int i = 0; i < 20 - len; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = '\0';
			begin++;
		}
		len = string(attr_list[j].Rel_Name).length();
		for (int i = 0; i < len; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = attr_list[j].Rel_Name[i];
			begin++;
		}
		for (int i = 0; i < 20 - len; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = '\0';
			begin++;
		}
		len = string(attr_list[j].Attr_Name).length();
		for (int i = 0; i < len; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = attr_list[j].Attr_Name[i];
			begin++;
		}
		for (int i = 0; i < 20 - len; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = '\0';
			begin++;
		}
		len = string(attr_list[j].Creator).length();
		for (int i = 0; i < len; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = attr_list[j].Creator[i];
			begin++;
		}
		for (int i = 0; i < 20 - len; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = '\0';
			begin++;
		}

		AttrType* attrtype_p = &attr_list[j].type;
		int* p;
		//int* p = &attr_list[j].attr_type;
		char* t_ch = (char*)attrtype_p;
		for (int i = 0; i < 4; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = t_ch[i];
			begin++;
		}
		p = &attr_list[j].Length;
		t_ch = (char*)p;
		for (int i = 0; i < 4; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = t_ch[i];
			begin++;
		}
		p = &attr_list[j].Offset;
		t_ch = (char*)p;
		for (int i = 0; i < 4; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = t_ch[i];
			begin++;
		}



		/*���棺��ʼδ�洢

		Distribution_Type* dis_type_p = &attr_list[j].distribution.type;
		t_ch = (char*)dis_type_p;
		for (int i = 0; i < 4; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = t_ch[i];
			begin++;
		}*/
		/*
		����
		ͬ��
		if (attr_list[j].distribution.type == Distribution_Type::NORMAL) {
			p = &attr_list[j].distribution.nor_eve.nor.mu;
			t_ch = (char*)p;
			for (int i = 0; i < 4; i++) {
				PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = t_ch[i];
				begin++;
			}
			p = &attr_list[j].distribution.nor_eve.nor.sigma;
			t_ch = (char*)p;
			for (int i = 0; i < 4; i++) {
				PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = t_ch[i];
				begin++;
			}


		}
		if (attr_list[j].distribution.type == Distribution_Type::EVENLY) {
			p = &attr_list[j].distribution.nor_eve.eve.MIN;
			t_ch = (char*)p;
			for (int i = 0; i < 4; i++) {
				PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = t_ch[i];
				begin++;
			}
			p = &attr_list[j].distribution.nor_eve.eve.MAX;
			t_ch = (char*)p;
			for (int i = 0; i < 4; i++) {
				PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = t_ch[i];
				begin++;
			}
		}
		*/


		Distribution* dis_p = &attr_list[j].distribution;
		t_ch = (char*)dis_p;
		for (int i = 0; i < 100; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = t_ch[i];
			begin++;
		}


		p = &attr_list[j].Num_of_Change_Records;
		t_ch = (char*)p;
		for (int i = 0; i < 4; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = t_ch[i];
			begin++;
		}

		p = &begin;
		t_ch = (char*)p;
		for (int i = 0; i < 4; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[i + 4] = t_ch[i];
		}


	}

	//PF_BufferMgr::pf_buffermgr.Write_buffer_to_page(buffer_id);
}

//ɾ�����ݱ�
bool Subsystem1_Manager::Delete_Rel(string RelName) {

	string com = "rd data\\" + UserName + "\\" + DBName + "\\" + RelName + " /s/q";
	system(com.c_str());
	Delete_Attr(RelName);



	int operate_page = 6;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			int* record_num = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* page_id_head = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			if (*exist == 1) {
				if (string(creator) == UserName && string(rel_name) == RelName && string(db_name) == DBName) {
					int temp_scan_begin = scan_begin - 72;
					int new_exist = 0;   int* new_exist_p = &new_exist;    char* new_exist_ch = (char*)(new_exist_p);
					for (int i = 0; i < 4; i++) {
						PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[temp_scan_begin] = new_exist_ch[i];
						temp_scan_begin++;
					}
					return true;
				}

			}

		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
	return false;
}


//�������ݱ������ֵΪ�Ƿ�ɹ�����������֮���Ĳ���rel�У�����ĺ���ͬ��
bool Subsystem1_Manager::lookup_Rel(string RelName, Rel_Info& rel) {
	int operate_page = 6;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			int* record_num = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* page_id_head = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			if (*exist == 1) {
				if (string(creator) == UserName && string(rel_name) == RelName && string(db_name) == DBName) {
					//cout << string(db_name) << " " << string(rel_name) << " " << string(creator) << " " << *record_num << " " << *page_id_head << endl;
					strcpy(rel.DBName, db_name); strcpy(rel.Rel_Name, rel_name); strcpy(rel.Creator, creator);
					rel.Record_Num = *record_num; rel.PageID_Head = *page_id_head;
					return true;
				}

			}

		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
	return false;
}

//��������
bool Subsystem1_Manager::lookup_Attr(string Rel_Name, string AttrName, Attr_Info& attr) {
	vector<Attr_Info> vec;
	int operate_page = 1;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Attribute", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* attr_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			int* attr_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* length = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* offset = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;


			/*����
			 ��ʼʱ����δ�洢��3��ֵ������ʹ�ýṹ�壨char����洢��

			int* distribution_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param1 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param2 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;*/

			char temp[1000];
			for (int i = 0; i < 100; i++) {
				temp[i] = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[scan_begin];
				scan_begin++;
			}
			Distribution* dis_p = (Distribution*)temp;

			int* num_of_change_records = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;

			if (string(db_name) == DBName && string(creator) == UserName && string(rel_name) == Rel_Name && string(attr_name) == AttrName && *exist == 1) {
				Attr_Info& attr_info = attr;
				strcpy(attr_info.Attr_Name, attr_name); strcpy(attr_info.Creator, creator); strcpy(attr_info.DBName, db_name);
				strcpy(attr_info.Rel_Name, rel_name);
				attr_info.Length = *length; attr_info.Offset = *offset; attr_info.Num_of_Change_Records = *num_of_change_records;
				if (*attr_type == 0) {
					attr_info.type = AttrType::INT;
				}
				else if (*attr_type == 1) {
					attr_info.type = AttrType::FLOAT;
				}
				else if (*attr_type == 2) {
					attr_info.type = AttrType::STRING;
				}

				attr_info.distribution = *dis_p;
				/*
				����
				�ź���ʵ��ʱ��distribution������double���д洢

				if (*distribution_type == 0) {
					attr_info.distribution.type = Distribution_Type::NORMAL;
					attr_info.distribution.nor_eve.nor.mu = *param1;
					attr_info.distribution.nor_eve.nor.sigma = *param2;
				}
				else {
					attr_info.distribution.type = Distribution_Type::EVENLY;
					attr_info.distribution.nor_eve.eve.MIN = *param1;
					attr_info.distribution.nor_eve.eve.MAX = *param2;
				}
				*/

				return true;
			}


		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
	return false;

}



//ɨ����ü�¼RID
vector<RID> Subsystem1_Manager::Scan_Record(string Rel_Name) {
	vector<RID> vec;  //�洢���з�������RID

	vector<Attr_Info> attr_vec = lookup_Attrs(Rel_Name); //�����Լ���
	if (attr_vec.size() == 0) {
		return vec;
	}
	for (int i = 0; i < attr_vec.size(); i++) {
		cout << attr_vec[i].Attr_Name << " ";
	}
	cout << endl;

	int len = 0;                         //�������Գ���֮��
	for (int i = 0; i < attr_vec.size(); i++) {
		len += attr_vec[i].Length;
	}

	int operate_page = Scan_rel_get_page_id(Rel_Name);
	string dir = "data\\" + UserName + "\\" + DBName + "\\" + Rel_Name;
	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer(dir, operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {


			//exist = 1 ��record���ڣ� = 0������
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);

			if (*exist == 1) {
				RID rid; rid.blockID = operate_page; rid.slotID = scan_begin;
				vec.push_back(rid);               //�洢RID
			}
			scan_begin += 4;



			//attr_vec[i]����ÿһ������  attr_vec[i].Attr_Name attr_vec[i].type ���Ի�����������������͵Ƚ��й���
			for (int i = 0; i < attr_vec.size(); i++) {

				if (attr_vec[i].type == AttrType::INT) {
					int* p = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
					if (*exist == 1) {
						cout << *p << " ";
					}
				}
				else if (attr_vec[i].type == AttrType::FLOAT) {
					float* p = (float*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
					if (*exist == 1) {
						cout << *p << " ";
					}

				}
				else {
					char* p = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
					if (*exist == 1) {
						cout << string(p) << " ";
					}

				}

			}


			if (*exist == 1) {
				cout << endl;
			}


		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
	return vec;
}

//��ȡ���page-id
int Subsystem1_Manager::Scan_rel_get_page_id(string Rel_Name) {
	int operate_page = 6;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			int* record_num = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* page_id_head = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			if (string(db_name) == DBName && string(creator) == UserName && string(rel_name) == Rel_Name && *exist == 1) {
				return *page_id_head;
			}
			/*if (*exist == 1) {
				cout << string(db_name) << " " << string(rel_name) << " " << string(creator) << " " << *record_num << " " << *page_id_head << endl;
			}*/

		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
}


char* Subsystem1_Manager::Find_Record_by_RID(string Rel_Name, RID rid) {
	vector<Attr_Info> attr_vec = lookup_Attrs(Rel_Name);
	int len = 0;
	for (int i = 0; i < attr_vec.size(); i++) {
		len += attr_vec[i].Length;
	}


	int operate_page = rid.blockID;
	string dir = "data\\" + UserName + "\\" + DBName + "\\" + Rel_Name;
	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer(dir, operate_page);
	char* ch = new char[len];
	for (int i = 0; i < len; i++) {
		ch[i] = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[rid.slotID + 4 + i];
	}
	return ch;

}

RID Subsystem1_Manager::Insert_Reocrd(string Rel_Name, char* record) {
	Change_Num_of_Change_Records(Rel_Name);
	Change_Rel_Record_num(Rel_Name, 1);


	int operate_page = Scan_rel_get_page_id(Rel_Name);

	string dir = "data/" + UserName + "/" + DBName + "/" + Rel_Name;
	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer(dir, operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (begin >= Global_Paras::Block_Size) {
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;

		if (operate_page == -1) {
			operate_page = Page_Mgr::page_mgr.Allocate_page_to_file(dir);

			int* p = &operate_page;
			char* t_ch = (char*)p;
			for (int i = 0; i < 4; i++) {
				PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[i] = t_ch[i];
			}

		}
		buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer(dir, operate_page);
		pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
		begin = *pp;
	}

	//-------------------------------------------------------------------------------------- -
	RID rid;
	rid.blockID = operate_page; rid.slotID = begin;
	int exist = 1;
	int* exist_p = &exist;
	char* exist_ch = (char*)exist_p;
	for (int i = 0; i < 4; i++) {
		PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = exist_ch[i];
		begin++;
	}



	vector<Attr_Info> attr_vec = lookup_Attrs(Rel_Name);
	int len = 0;
	for (int i = 0; i < attr_vec.size(); i++) {
		len += attr_vec[i].Length;
	}
	for (int i = 0; i < len; i++) {
		PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = record[i];
		begin++;
	}

	int* p = &begin;
	char* t_ch = (char*)p;
	for (int i = 0; i < 4; i++) {
		PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[i + 4] = t_ch[i];
	}
	PF_BufferMgr::pf_buffermgr.Write_buffer_to_page(buffer_id);

	return rid;

}

void Subsystem1_Manager::Delete_Record(string Rel_Name, vector<RID> rids) {
	Change_Num_of_Change_Records(Rel_Name);
	Change_Rel_Record_num(Rel_Name, -1);

	for (RID rid : rids) {

		int operate_page = rid.blockID;
		string dir = "data\\" + UserName + "\\" + DBName + "\\" + Rel_Name;
		int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer(dir, operate_page);
		int new_exist = 0;   int* new_exist_p = &new_exist;    char* new_exist_ch = (char*)(new_exist_p);
		int temp_scan_begin = rid.slotID;
		for (int i = 0; i < 4; i++) {
			PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[temp_scan_begin] = new_exist_ch[i];
			temp_scan_begin++;
		}
	}
}

void Subsystem1_Manager::Update_Record(string Rel_Name, RID rid, Attr_Info attr, char* new_value) {
	Change_Num_of_Change_Records(Rel_Name, attr.Attr_Name);

	int Length = attr.Length;
	int Offset = attr.Offset;


	int operate_page = rid.blockID;
	int slot_id = rid.slotID;
	string dir = "data\\" + UserName + "\\" + DBName + "\\" + Rel_Name;
	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer(dir, operate_page);

	int temp_scan_begin = slot_id + Offset + sizeof(int);
	for (int i = 0; i < Length; i++) {
		PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[temp_scan_begin] = new_value[i];
		temp_scan_begin++;
	}


}




//---------------------------------------------------------------�����Ϸ�����ʹ�ý϶�




//��������
bool Subsystem1_Manager::lookup_Index(string RelName, string AttrName, Index_Info& Index) {
	int operate_page = 4;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Index", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* attr_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* index_creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			int* page_id_head = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* index_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			if (*exist == 1) {
				if (string(rel_name) == RelName) {
					Index_Info& index_info = Index;
					strcpy(index_info.Rel_Name, rel_name); strcpy(index_info.Creator, creator); strcpy(index_info.DBName, db_name);
					strcpy(index_info.Attr_Name, attr_name);  strcpy(index_info.Index_Creator, index_creator);
					//cout << string(db_name) << " " << string(rel_name) << " " << string(attr_name) << " " << string(creator) << " " << string(index_creator) << " " << *page_id_head << " ";
					if (*index_type == 0) {
						//cout << "CLUSTER_INDEX" << endl;
						index_info.type = IndexType::CLUSTER_INDEX;
					}
					else {
						//cout << "NORMAL_INDEX" << endl;
						index_info.type = IndexType::NORMAL_INDEX;
					}
					return true;
				}

			}

		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
	return false;
}

//������������
vector<Index_Info> Subsystem1_Manager::lookup_Indexes(string RelName) {
	vector<Index_Info> index_vec;
	int operate_page = 4;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Index", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* attr_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* index_creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			int* page_id_head = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* index_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			if (*exist == 1) {
				if (string(rel_name) == RelName) {
					Index_Info index_info;
					strcpy(index_info.Rel_Name, rel_name); strcpy(index_info.Creator, creator); strcpy(index_info.DBName, db_name);
					strcpy(index_info.Attr_Name, attr_name);  strcpy(index_info.Index_Creator, index_creator);
					//cout << string(db_name) << " " << string(rel_name) << " " << string(attr_name) << " " << string(creator) << " " << string(index_creator) << " " << *page_id_head << " ";
					if (*index_type == 0) {
						//cout << "CLUSTER_INDEX" << endl;
						index_info.type = IndexType::CLUSTER_INDEX;
					}
					else {
						//cout << "NORMAL_INDEX" << endl;
						index_info.type = IndexType::NORMAL_INDEX;
					}
					index_vec.push_back(index_info);
				}

			}

		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
	return index_vec;
}

//�޸ļ�¼����
bool Subsystem1_Manager::Change_Rel_Record_num(string RelName, int change_num) {
	int operate_page = 6;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			int record_num_scan = scan_begin;
			int* record_num = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* page_id_head = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			if (*exist == 1) {
				if (string(creator) == UserName && string(rel_name) == RelName && string(db_name) == DBName) {
					//cout << string(db_name) << " " << string(rel_name) << " " << string(creator) << " " << *record_num << " " << *page_id_head << endl;
					int new_record_num = *record_num + change_num;
					int* new_record_num_p = &new_record_num;
					char* new_record_num_ch = (char*)new_record_num_p;
					for (int i = 0; i < 4; i++) {
						PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[record_num_scan] = new_record_num_ch[i];
						record_num_scan++;
					}
					return true;
				}

			}

		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
	return false;
}

//ɾ������
void Subsystem1_Manager::Delete_Attr(string Rel_Name) {
	vector<Attr_Info> vec;
	int operate_page = 1;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Attribute", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int exist_scan = scan_begin;
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* attr_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			int* attr_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* length = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* offset = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;


			/*����
			 ��ʼʱ����δ�洢��3��ֵ������ʹ�ýṹ�壨char����洢��

			int* distribution_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param1 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param2 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;*/

			char temp[1000];
			for (int i = 0; i < 100; i++) {
				temp[i] = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[scan_begin];
				scan_begin++;
			}
			Distribution* dis_p = (Distribution*)temp;

			int* num_of_change_records = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;

			if (string(db_name) == DBName && string(creator) == UserName && string(rel_name) == Rel_Name && *exist == 1) {
				int new_exist = 0;
				int* new_exist_p = &new_exist;
				char* new_exist_ch = (char*)new_exist_p;
				for (int i = 0; i < 4; i++) {
					PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[exist_scan] = new_exist_ch[i];
					exist_scan++;
				}

				/*
				����
				�ź���ʵ��ʱ��distribution������double���д洢

				if (*distribution_type == 0) {
					attr_info.distribution.type = Distribution_Type::NORMAL;
					attr_info.distribution.nor_eve.nor.mu = *param1;
					attr_info.distribution.nor_eve.nor.sigma = *param2;
				}
				else {
					attr_info.distribution.type = Distribution_Type::EVENLY;
					attr_info.distribution.nor_eve.eve.MIN = *param1;
					attr_info.distribution.nor_eve.eve.MAX = *param2;
				}
				*/

			}


		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}

}

//�޸�Num_of_Change_Records
bool Subsystem1_Manager::Change_Num_of_Change_Records(string Rel_Name, string AttrName) {
	vector<Attr_Info> vec;
	int operate_page = 1;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Attribute", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* attr_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			int* attr_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* length = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* offset = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;


			/*����
			 ��ʼʱ����δ�洢��3��ֵ������ʹ�ýṹ�壨char����洢��

			int* distribution_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param1 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param2 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;*/
			char temp[1000];
			for (int i = 0; i < 100; i++) {
				temp[i] = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[scan_begin];
				scan_begin++;
			}
			Distribution* dis_p = (Distribution*)temp;


			int num_of_change_records_scan = scan_begin;
			int* num_of_change_records = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;

			if (string(db_name) == DBName && string(creator) == UserName && string(rel_name) == Rel_Name && string(attr_name) == AttrName && *exist == 1) {

				int new_num_of_change_records = *num_of_change_records + 1;
				int* new_num_of_change_records_p = &new_num_of_change_records;
				char* new_num_of_change_records_ch = (char*)new_num_of_change_records_p;
				for (int i = 0; i < 4; i++) {
					PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[num_of_change_records_scan] = new_num_of_change_records_ch[i];
					num_of_change_records_scan++;
				}


				/*
				����
				�ź���ʵ��ʱ��distribution������double���д洢

				if (*distribution_type == 0) {
					attr_info.distribution.type = Distribution_Type::NORMAL;
					attr_info.distribution.nor_eve.nor.mu = *param1;
					attr_info.distribution.nor_eve.nor.sigma = *param2;
				}
				else {
					attr_info.distribution.type = Distribution_Type::EVENLY;
					attr_info.distribution.nor_eve.eve.MIN = *param1;
					attr_info.distribution.nor_eve.eve.MAX = *param2;
				}
				*/

				return true;
			}


		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
	return false;

}

//�޸�Num_of_Change_Records
bool Subsystem1_Manager::Change_Num_of_Change_Records(string Rel_Name) {
	vector<Attr_Info> vec;
	int operate_page = 1;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Attribute", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* attr_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			int* attr_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* length = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* offset = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;


			/*����
			 ��ʼʱ����δ�洢��3��ֵ������ʹ�ýṹ�壨char����洢��

			int* distribution_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param1 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param2 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;*/

			char temp[1000];
			for (int i = 0; i < 100; i++) {
				temp[i] = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[scan_begin];
				scan_begin++;
			}
			Distribution* dis_p = (Distribution*)temp;

			int num_of_change_records_scan = scan_begin;
			int* num_of_change_records = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;

			if (string(db_name) == DBName && string(creator) == UserName && string(rel_name) == Rel_Name && *exist == 1) {

				int new_num_of_change_records = *num_of_change_records + 1;
				int* new_num_of_change_records_p = &new_num_of_change_records;
				char* new_num_of_change_records_ch = (char*)new_num_of_change_records_p;
				for (int i = 0; i < 4; i++) {
					PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[num_of_change_records_scan] = new_num_of_change_records_ch[i];
					num_of_change_records_scan++;
				}


				/*
				����
				�ź���ʵ��ʱ��distribution������double���д洢

				if (*distribution_type == 0) {
					attr_info.distribution.type = Distribution_Type::NORMAL;
					attr_info.distribution.nor_eve.nor.mu = *param1;
					attr_info.distribution.nor_eve.nor.sigma = *param2;
				}
				else {
					attr_info.distribution.type = Distribution_Type::EVENLY;
					attr_info.distribution.nor_eve.eve.MIN = *param1;
					attr_info.distribution.nor_eve.eve.MAX = *param2;
				}
				*/

			}


		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
	return false;

}

//��ѯȨ�� ret[SELECT]=true,ret[UPDATE]=false�����ݱ���û������ڷ���NULL
bool* Subsystem1_Manager::lookup_Authority(string RelName, string UserName) {
	int operate_page = 2;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Authority", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* user_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			bool* autho = (bool*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin); scan_begin += 4;
			if (*exist == 1) {
				if (string(rel_name) == RelName && string(user_name) == UserName) {
					return autho;
				}
			}

		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
	return NULL;
}

void Subsystem1_Manager::set_change_records(Attr_Info attr, int num) {
	vector<Attr_Info> vec;
	int operate_page = 1;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Attribute", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* attr_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			int* attr_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* length = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* offset = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;


			/*警告
			 初始时，并未存储这3个值，后续使用结构体（char数组存储）
			int* distribution_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param1 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param2 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;*/

			char temp[1000];
			for (int i = 0; i < 100; i++) {
				temp[i] = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[scan_begin];
				scan_begin++;
			}
			Distribution* dis_p = (Distribution*)temp;


			int num_of_change_records_scan = scan_begin;
			int* num_of_change_records = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;

			if (string(db_name) == DBName && string(creator) == UserName && string(rel_name) == attr.Rel_Name && string(attr_name) == attr.Attr_Name && *exist == 1) {

				int new_num_of_change_records = num;
				int* new_num_of_change_records_p = &new_num_of_change_records;
				char* new_num_of_change_records_ch = (char*)new_num_of_change_records_p;
				for (int i = 0; i < 4; i++) {
					PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[num_of_change_records_scan] = new_num_of_change_records_ch[i];
					num_of_change_records_scan++;
				}


				/*
				警告
				张浩在实现时将distribution以两个double进行存储
				if (*distribution_type == 0) {
					attr_info.distribution.type = Distribution_Type::NORMAL;
					attr_info.distribution.nor_eve.nor.mu = *param1;
					attr_info.distribution.nor_eve.nor.sigma = *param2;
				}
				else {
					attr_info.distribution.type = Distribution_Type::EVENLY;
					attr_info.distribution.nor_eve.eve.MIN = *param1;
					attr_info.distribution.nor_eve.eve.MAX = *param2;
				}
				*/

			}


		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
	return;
}



void Subsystem1_Manager::set_distribution(Attr_Info attr, Distribution distribution) {
	char temp_[1000];
	Distribution* dis_p = &distribution;
	char* temp__ = (char*)dis_p;
	for (int i = 0; i < 100; i++) {
		temp_[i] = temp__[i];
	}

	memcpy(temp_, &distribution, sizeof(Distribution));

	vector<Attr_Info> vec;
	int operate_page = 1;

	int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Attribute", operate_page);
	int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
	int begin = *pp;

	while (operate_page != -1) {
		int scan_begin = 8;
		while (scan_begin != begin) {
			int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			char* db_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin; scan_begin += 20;
			char* rel_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* attr_name = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			char* creator = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin;  scan_begin += 20;
			int* attr_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* length = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* offset = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;


			/*警告
			 初始时，并未存储这3个值，后续使用结构体（char数组存储）
			int* distribution_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param1 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
			int* param2 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;*/

			int dis_scan_begin = scan_begin;
			char temp[100];
			for (int i = 0; i < 100; i++) {
				temp[i] = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[scan_begin];
				scan_begin++;
			}
			Distribution* dis_p = (Distribution*)temp;

			int* num_of_change_records = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;

			if (string(db_name) == DBName && string(creator) == UserName && string(rel_name) == attr.Rel_Name && string(attr_name) == attr.Attr_Name && *exist == 1) {

				for (int i = 0; i < 100; i++) {
					PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[dis_scan_begin] = temp_[i];
					dis_scan_begin++;
				}

				return;
			}


		}
		int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
		operate_page = *pl;
		if (operate_page != -1) {
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			begin = *pp;
		}

	}
	return;
}


Subsystem1_Manager Subsystem1_Manager::mgr;
