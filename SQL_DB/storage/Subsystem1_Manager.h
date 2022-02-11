#pragma once
#include"Page_Mgr.h"


class Subsystem1_Manager {
private:
public:
	static Subsystem1_Manager mgr;
	string UserName;
	string DBName;
	//扫描所有表
	void Scan_rel();
	//扫描所有属性
	void Scan_attribute();
	//查找所有属性
	vector<Attr_Info> lookup_Attrs(string Rel_Name);
	//新建数据表
	void Create_Rel(string RelName, vector<Attr_Info> attrs);
	void Add_rel(Rel_Info rel_info);
	void Add_attribute(vector<Attr_Info> attr_list);
	//删除数据表
	bool Delete_Rel(string RelName);
	//查找数据表，返回值为是否成功，将结果存放之最后的参数rel中，后面的函数同理
	bool lookup_Rel(string RelName, Rel_Info& rel);
	//查找属性
	bool lookup_Attr(string Rel_Name, string AttrName, Attr_Info& attr);
	//扫描表获得记录RID
	vector<RID> Scan_Record(string Rel_Name);
	//获取表的page-id
	int Scan_rel_get_page_id(string Rel_Name);
	char* Find_Record_by_RID(string Rel_Name, RID rid);
	RID Insert_Reocrd(string Rel_Name, char* record);
	void Delete_Record(string Rel_Name, vector<RID> rids);
	void Update_Record(string Rel_Name, RID rid, Attr_Info attr, char* new_value);
	//查找索引
	bool lookup_Index(string RelName, string AttrName, Index_Info& Index);
	//查找所有索引
	vector<Index_Info> lookup_Indexes(string RelName);
	//修改记录数量
	bool Change_Rel_Record_num(string RelName, int change_num);
	//删除属性
	void Delete_Attr(string Rel_Name);
	//修改Num_of_Change_Records
	bool Change_Num_of_Change_Records(string Rel_Name, string AttrName);
	//修改Num_of_Change_Records
	bool Change_Num_of_Change_Records(string Rel_Name);
	//查询权限 ret[SELECT]=true,ret[UPDATE]=false，数据表或用户不存在返回NULL
	bool* lookup_Authority(string RelName, string UserName);
	void set_distribution(Distribution distribution);
};

