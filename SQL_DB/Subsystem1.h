#pragma once
#include "storage/Subsystem1_Manager.h"
#include "global.h"
#include "parser/parser_interp.h"




class Reader {  //һ�����ڱ������ݱ����
public:
	//��ȡ��һ�����ݣ�����char*Ϊԭ���ĸ���
	virtual char* get_Next_Record() = 0;
	//��ȡ��һ�����ݣ�����char*Ϊԭ���ĸ��ƣ�rid��ֵΪ�����ݵ�RID
	virtual char* get_Next_Record_with_RID(RID& rid) = 0;
};

class Scan_Reader :public Reader {
private:
	vector<Condition> Conds;
	vector<RID> RIDS;
	int index;
	string RelName;
	//������
	bool filter(char* record) {
		return false;
	}
public:
	Scan_Reader(string RelName, vector<Condition> Conds= vector<Condition>()) {
		this->RelName = RelName;
		RIDS = Subsystem1_Manager::mgr.Scan_Record(RelName);
		index = 0;
	}
	//��ȡ��һ�����ݣ�����char*Ϊԭ���ĸ��ƣ�����ֵ��������filter
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
	//��ȡ��һ�����ݣ�����char*Ϊԭ���ĸ��ƣ�rid��ֵΪ�����ݵ�RID
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
	//MIN/MAX==NULL�������ޣ�MIN_E��MAX_E�����ܷ�ȡ��
	Index_Reader(string RelName, Index_Info Index, bool MIN_E, char* MIN, bool MAX_E, char* MAX) {

	}
	//��ȡ��һ�����ݣ�����char*Ϊԭ���ĸ���
	char* get_Next_Record() {
		return NULL;
	}
	//��ȡ��һ�����ݣ�����char*Ϊԭ���ĸ��ƣ�rid��ֵΪ�����ݵ�RID
	char* get_Next_Record_with_RID(RID& rid) {
		return NULL;
	}
};