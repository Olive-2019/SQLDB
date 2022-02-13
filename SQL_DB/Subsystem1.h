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
	Scan_Reader(string RelName, vector<Condition> Conds = vector<Condition>()) {
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
	double get_mu(Attr_Info attr) {
		int before_index = this->index;
		this->index = 0;
		char* record;
		int num = 0;
		double sum = 0;
		while ((record = get_Next_Record()) != NULL) {
			num++;
			char* value = record + attr.Offset;
			if (attr.type == INT) {
				sum += *(int*)value;
			}
			else if (attr.type == FLOAT) {
				sum += *(float*)value;
			}

		}
		double avg = sum / num;
		this->index = before_index;
		return avg;
	}
	double get_sigma(Attr_Info attr) {
		double avg = get_mu(attr);
		int before_index = this->index;
		this->index = 0;
		char* record;
		int num = 0;
		double sum = 0;
		while ((record = get_Next_Record()) != NULL) {
			num++;
			char* value = record + attr.Offset;
			if (attr.type == INT) {
				double temp = avg - *(int*)value;
				sum += temp * temp;
			}
			else if (attr.type == FLOAT) {
				double temp = avg - *(float*)value;
				sum += temp * temp;
			}

		}
		double var = sum / num;
		this->index = before_index;
		return var;
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