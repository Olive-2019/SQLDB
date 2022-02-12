#pragma once
#include"Page_Mgr.h"


class Subsystem1_Manager {
private:

public:
	Subsystem1_Manager() {

	}
	static Subsystem1_Manager mgr;
	string UserName;
	string DBName;


	//ɨ�����б�
	void Scan_rel();

	//ɨ����������
	void Scan_attribute();
	//������������
	vector<Attr_Info> lookup_Attrs(string Rel_Name);


	//�½����ݱ�
	void Create_Rel(string RelName, vector<Attr_Info> attrs);

	void Add_rel(Rel_Info rel_info);
	void Add_attribute(vector<Attr_Info> attr_list);

	//ɾ�����ݱ�
	bool Delete_Rel(string RelName);


	//�������ݱ������ֵΪ�Ƿ�ɹ�����������֮���Ĳ���rel�У�����ĺ���ͬ��
	bool lookup_Rel(string RelName, Rel_Info& rel);

	//��������
	bool lookup_Attr(string Rel_Name, string AttrName, Attr_Info& attr);



	//ɨ����ü�¼RID
	vector<RID> Scan_Record(string Rel_Name);

	//��ȡ���page-id
	int Scan_rel_get_page_id(string Rel_Name);


	char* Find_Record_by_RID(string Rel_Name, RID rid);

	RID Insert_Reocrd(string Rel_Name, char* record);

	void Delete_Record(string Rel_Name, vector<RID> rids);

	void Update_Record(string Rel_Name, RID rid, Attr_Info attr, char* new_value);




	//---------------------------------------------------------------�����Ϸ�����ʹ�ý϶�




	//��������
	bool lookup_Index(string RelName, string AttrName, Index_Info& Index);

	//������������
	vector<Index_Info> lookup_Indexes(string RelName);

	//�޸ļ�¼����
	bool Change_Rel_Record_num(string RelName, int change_num);

	//ɾ������
	void Delete_Attr(string Rel_Name);

	//�޸�Num_of_Change_Records
	bool Change_Num_of_Change_Records(string Rel_Name, string AttrName);

	//�޸�Num_of_Change_Records
	bool Change_Num_of_Change_Records(string Rel_Name);

	//��ѯȨ�� ret[SELECT]=true,ret[UPDATE]=false�����ݱ���û������ڷ���NULL
	bool* lookup_Authority(string RelName, string UserName);




	void set_distribution(Attr_Info attr, Distribution distribution);

	//����attr.Num_of_Change_Records = 0;
	void set_change_records(Attr_Info attr, int num);




};
