#pragma once
#include "../global.h"


//每一个缓冲区
class PF_BufPageDesc {
public:
	PF_BufPageDesc() {
		pData = new char[Global_Paras::Block_Size];
	}
	//缓冲区存储page的dir
	string dir;
	int    pageNum;

	char* pData;
	//用来LRU
	int time;

};

//缓冲区管理
class PF_BufferMgr {
public:
	static PF_BufferMgr pf_buffermgr;
	//读取page到缓冲区
	int Read_page_to_buffer(string dir, int pageNum) {
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
	int Write_buffer_to_page(int buffer_id) {

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
	int Write_all_buffer() {
		for (int i = 0; i < occupy_buffer_size; i++) {
			Write_buffer_to_page(i);
		}
		return 0;
	}


	//判断page是否在缓冲区
	int If_page_in_buffer(int pageNum) {
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
	int LRU_get_buffer_id(string insert_dir, int insert_pageNum) {
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

public:
	//缓冲区大小
	int occupy_buffer_size;

	//缓冲区集合
	PF_BufPageDesc buffer_list[BUFFER_NUM];

	//page所在缓冲区
	unordered_map<int, int> Page_to_buffer_mp;
};


//管理空闲页面
class Page_Mgr {
public:
	static Page_Mgr page_mgr;
	Page_Mgr() {
		FILE* fp;
		string filename = "sys/0";
		if ((fp = fopen(filename.c_str(), "ab+")) == NULL) {
			cout << "asdas" << endl;
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
	int Write_page() {
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
	int Allocate_page_to_file(string dir) {
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
	int Recover_page(string dir, int Page_id) {
		free_page_id_list.push_back(Page_id);
		string filename = dir + to_string(Page_id);
		remove(filename.c_str());
		return 0;
	}

	//下一个分配的page-id
	int allocate_page_id;
	vector<int> free_page_id_list;
};



class Subsystem1_Manager {
private:

public:
	Subsystem1_Manager() {
		UserName = "User1";
		DBName = "DB1";
	}
	static Subsystem1_Manager mgr;
	string UserName;
	string DBName;
	//查找数据表，返回值为是否成功，将结果存放之最后的参数rel中，后面的函数同理
	bool lookup_Rel(string RelName, Rel_Info& rel) {
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

	//查找属性
	bool lookup_Attr(string Rel_Name, string AttrName, Attr_Info& attr) {
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
				int* distribution_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
				int* param1 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
				int* param2 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
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

	//查找所有属性
	vector<Attr_Info> lookup_Attrs(string Rel_Name) {
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
				int* distribution_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
				int* param1 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
				int* param2 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
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
					/*
					警告
					同上
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
	//查询权限 ret[SELECT]=true,ret[UPDATE]=false，数据表或用户不存在返回NULL
	bool* lookup_Authority(string RelName, string UserName) {
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

	//查找索引
	bool lookup_Index(string RelName, string AttrName, Index_Info& Index) {
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

	//查找所有索引
	vector<Index_Info> lookup_Indexes(string RelName) {
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

	//获取表数据
	vector<char*> Find_Record(string Rel_Name) {
		vector<char*> vec;
		vector<Attr_Info> attr_vec = lookup_Attrs(Rel_Name);
		for (int i = 0; i < attr_vec.size(); i++) {
			cout << attr_vec[i].Attr_Name << " ";
		}
		cout << endl;
		int len = 0;
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
				//exist =1 该record存在，=0不存在
				int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
				//记录加入到vector中，作为最终返回值
				if (*exist == 1) {
					int temp_scan_begin = scan_begin;
					char* ch = new char[len];
					for (int i = 0; i < len; i++) {
						ch[i] = PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[temp_scan_begin];
						temp_scan_begin++;
					}
					vec.push_back(ch);
				}
				//attr_vec[i]代表每一个属性
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


	//删除表数据
	void Delete_Record(string Rel_Name) {
		vector<Attr_Info> attr_vec = lookup_Attrs(Rel_Name);
		for (int i = 0; i < attr_vec.size(); i++) {
			cout << attr_vec[i].Attr_Name << " ";
		}
		cout << endl;

		int operate_page = Scan_rel_get_page_id(Rel_Name);
		string dir = "data\\" + UserName + "\\" + DBName + "\\" + Rel_Name;
		int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer(dir, operate_page);
		int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
		int begin = *pp;

		while (operate_page != -1) {
			int scan_begin = 8;
			while (scan_begin != begin) {

				int new_exist = 0;   int* new_exist_p = &new_exist;    char* new_exist_ch = (char*)(new_exist_p);
				int temp_scan_begin = scan_begin;
				for (int i = 0; i < 4; i++) {
					PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[temp_scan_begin] = new_exist_ch[i];
					temp_scan_begin++;
				}
				int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin - 4);  scan_begin += 4;


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
	}

	//更新表数据
	void Update_Record(string Rel_Name, char* new_value) {
		vector<Attr_Info> attr_vec = lookup_Attrs(Rel_Name);
		for (int i = 0; i < attr_vec.size(); i++) {
			cout << attr_vec[i].Attr_Name << " ";
		}
		cout << endl;

		int len = 0;
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
				int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
				if (*exist == 1) {
					int temp_scan_begin = scan_begin;
					for (int i = 0; i < len; i++) {
						PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[temp_scan_begin] = new_value[i];
						temp_scan_begin++;
					}
				}
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
	}

	//新建数据表
	void Create_Rel(string RelName, vector<Attr_Info> attrs) {
		Rel_Info rel_info;
		memcpy(rel_info.Rel_Name, RelName.c_str(), RelName.length() + 1);
		memcpy(rel_info.DBName, DBName.c_str(), DBName.length() + 1);
		memcpy(rel_info.Creator, UserName.c_str(), UserName.length() + 1);
		Add_rel(rel_info);
		Add_attribute(attrs);
	}

	void Add_rel(Rel_Info rel_info) {

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
			int begin = *pp;
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

		//Add_attribute(rel_info.attr_list); 注意


	}
	void Add_attribute(vector<Attr_Info> attr_list) {
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
			int begin = *pp;
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



			Distribution_Type* dis_type_p = &attr_list[j].distribution.type;
			t_ch = (char*)dis_type_p;
			for (int i = 0; i < 4; i++) {
				PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[begin] = t_ch[i];
				begin++;
			}
			/*
			警告
			同上
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

	//删除数据表
	bool Delete_Rel(string RelName) {
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

	//获取表的page-id
	int Scan_rel_get_page_id(string Rel_Name) {
		int operate_page = 6;

		int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer("sys/Relation", operate_page);
		int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
		int begin = *pp;
		cout << "saddas==" << operate_page << endl;
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

	//扫描所有表
	void Scan_rel() {
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

	//扫描所有属性
	void Scan_attribute() {
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
				int* distribution_type = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
				int* param1 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
				int* param2 = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);  scan_begin += 4;
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
					if (*distribution_type == 0) {
						cout << "NORMAL::" << *param1 << " " << *param2;
					}
					else {
						cout << "EVENLY::" << *param1 << " " << *param2;;
					}
					cout << " " << *num_of_change_records << endl;
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



	char* Find_Record_by_RID(string Rel_Name, RID rid) {
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

	RID Insert_Reocrd(string Rel_Name, char* record) {
		int operate_page = Scan_rel_get_page_id(Rel_Name);

		string dir = "../data/" + UserName + "/" + DBName + "/" + Rel_Name;
		FILE* file = fopen(dir.c_str(), "a");
		if (file == NULL) {
			cout << "NULL" << endl;
			exit(0);
		}
		cout << "dir==" << dir << endl;
		int buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer(dir, operate_page);
		int* pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
		int begin = *pp;
		cout << "1" << endl;
		while (begin >= Global_Paras::Block_Size) {
			int* pl = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData);
			operate_page = *pl;
			cout << "operate page==" << operate_page << endl;
			if (operate_page == -1) {
				operate_page = Page_Mgr::page_mgr.Allocate_page_to_file(dir);
				cout << "new operate page==" << operate_page << endl;
				int* p = &operate_page;
				char* t_ch = (char*)p;
				for (int i = 0; i < 4; i++) {
					PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData[i] = t_ch[i];
				}

			}
			buffer_id = PF_BufferMgr::pf_buffermgr.Read_page_to_buffer(dir, operate_page);
			pp = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + 4);
			int begin = *pp;
		}
		cout << "2" << endl;
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


		cout << "3" << endl;
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

	void Delete_Record(string Rel_Name, vector<RID> rids) {
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

	void Update_Record(string Rel_Name, RID rid, Attr_Info attr, char* new_value) {

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

	//扫描表获得记录RID
	vector<RID> Scan_Record(string Rel_Name) {
		vector<RID> vec;  //存储所有符合条件RID

		vector<Attr_Info> attr_vec = lookup_Attrs(Rel_Name); //表属性集合
		for (int i = 0; i < attr_vec.size(); i++) {
			cout << attr_vec[i].Attr_Name << " ";
		}
		cout << endl;

		int len = 0;                         //所有属性长度之和
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


				//exist = 1 该record存在， = 0不存在
				int* exist = (int*)(PF_BufferMgr::pf_buffermgr.buffer_list[buffer_id].pData + scan_begin);

				if (*exist == 1) {
					RID rid; rid.blockID = operate_page; rid.slotID = scan_begin;
					vec.push_back(rid);               //存储RID
				}
				scan_begin += 4;



				//attr_vec[i]代表每一个属性  attr_vec[i].Attr_Name attr_vec[i].type 可以获得属性名、属性类型等进行过滤
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


};



