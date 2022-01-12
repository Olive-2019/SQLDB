#pragma once
#include "global.h"
#include "parser/parser_interp.h"
class Subsystem1_Manager {
private:
	Subsystem1_Manager() {

	}
public:
	static Subsystem1_Manager BASE;

	//�������ݱ�����ֵΪ�Ƿ�ɹ�����������֮���Ĳ���rel�У�����ĺ���ͬ��
	bool lookup_Rel(string RelName, Rel_Info& rel);

	//��������
	bool lookup_Attr(string RelName, string AttrName, Attr_Info& attr);

	//������������
	vector<Attr_Info> lookup_Attrs(string RelName);

	//��ѯȨ�� ret[SELECT]=true,ret[UPDATE]=false�����ݱ���û������ڷ���NULL
	bool* lookup_Authority(string RelName, string UserName);

	//��������
	bool lookup_Index(string RelName, string AttrName, Index_Info& Index);

	//������������
	vector<Index_Info> lookup_Indexes(string RelName);

	//ͨ��RID�ҵ�����,����char*Ϊԭ���ĸ��ƣ������ڷ���NULL
	char* Find_Record_by_RID(RID rid);

	//��������
	void Insert_Reocrd(string RelName, char* record);

	//����RIDɾ������
	void Delete_Record(string RelName, vector<RID> rids);

	//����RID��������
	void Update_Record(string RelName, RID rid, Attr_Info attr, char* new_value);
};

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
	bool filter(char* record);
public:
	Scan_Reader(string RelName, vector<Condition> Conds);
	//��ȡ��һ�����ݣ�����char*Ϊԭ���ĸ��ƣ�����ֵ��������filter
	char* get_Next_Record();
	//��ȡ��һ�����ݣ�����char*Ϊԭ���ĸ��ƣ�rid��ֵΪ�����ݵ�RID
	char* get_Next_Record_with_RID(RID& rid);
};

class Index_Reader :public Reader {
public:
	//MIN/MAX==NULL�������ޣ�MIN_E��MAX_E�����ܷ�ȡ��
	Index_Reader(string RelName, Index_Info Index, bool MIN_E, char* MIN, bool MAX_E, char* MAX);
	//��ȡ��һ�����ݣ�����char*Ϊԭ���ĸ���
	char* get_Next_Record();
	//��ȡ��һ�����ݣ�����char*Ϊԭ���ĸ��ƣ�rid��ֵΪ�����ݵ�RID
	char* get_Next_Record_with_RID(RID& rid);
};