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
	Scan_Reader(string RelName, vector<Condition> Conds = vector<Condition>()) {
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