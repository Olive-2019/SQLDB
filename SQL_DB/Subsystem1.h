#pragma once
#include "storage/Subsystem1_Manager.h"
#include "global.h"
#include "parser/parser_interp.h"




class Reader {  //一个用于遍历数据表的类
public:
	//获取下一条数据，返回char*为原件的复制
	virtual char* get_Next_Record() = 0;
	//获取下一条数据，返回char*为原件的复制，rid赋值为该数据的RID
	virtual char* get_Next_Record_with_RID(RID& rid) = 0;
};

class Scan_Reader :public Reader {
private:
	vector<Condition> Conds;
	vector<RID> RIDS;
	int index;
	string RelName;
	//过滤器
	bool filter(char* record) {
		return false;
	}
public:
	Scan_Reader(string RelName, vector<Condition> Conds= vector<Condition>()) {
		this->RelName = RelName;
		RIDS = Subsystem1_Manager::mgr.Scan_Record(RelName);
		index = 0;
	}
	//获取下一条数据，返回char*为原件的复制，返回值必须满足filter
	char* get_Next_Record() {

		if (index == RIDS.size()) {
			return NULL;
		}
		RID rid = RIDS[index++];
		char* record = Subsystem1_Manager::mgr.Find_Record_by_RID(RelName, rid);
		char* ret = new char[1000];
		memcpy(ret, record, 1000);
		return ret;
	}
	//获取下一条数据，返回char*为原件的复制，rid赋值为该数据的RID
	char* get_Next_Record_with_RID(RID& rid) {
		if (index == RIDS.size()) {
			return NULL;
		}
		rid = RIDS[index++];
		char* record = Subsystem1_Manager::mgr.Find_Record_by_RID(RelName, rid);
		char* ret = new char[1000];
		memcpy(ret, record, 1000);
		return ret;
	}
};

class Index_Reader :public Reader {
public:
	//MIN/MAX==NULL代表不受限，MIN_E与MAX_E代表能否取等
	Index_Reader(string RelName, Index_Info Index, bool MIN_E, char* MIN, bool MAX_E, char* MAX) {

	}
	//获取下一条数据，返回char*为原件的复制
	char* get_Next_Record() {
		return NULL;
	}
	//获取下一条数据，返回char*为原件的复制，rid赋值为该数据的RID
	char* get_Next_Record_with_RID(RID& rid) {
		return NULL;
	}
};