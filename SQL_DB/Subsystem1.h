#pragma once
#include "storage/pf.h"
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
	//������
	bool filter(char* record) {
		return false;
	}
public:
	Scan_Reader(string RelName, vector<Condition> Conds= vector<Condition>()) {}
	//��ȡ��һ�����ݣ�����char*Ϊԭ���ĸ��ƣ�����ֵ��������filter
	char* get_Next_Record() {
		return NULL;
	}
	//��ȡ��һ�����ݣ�����char*Ϊԭ���ĸ��ƣ�rid��ֵΪ�����ݵ�RID
	char* get_Next_Record_with_RID(RID& rid) {
		return NULL;
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