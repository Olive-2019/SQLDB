#pragma once
#include "global.h"
class Subsystem1_Manager {

public:

	//查找数据表
	Rel_Info lookup_Rel(string RelName);

	//查找属性
	Attr_Info lookup_Attr(string RelName, string AttrName);

	//查找所有属性
	vector<Attr_Info> lookup_Attrs(string RelName);

	//查询权限
	bool* lookup_Authority(string RelName, string UserName);

	//查找索引
	Index_Info lookup_Index(string RelName, string AttrName);

	//查找所有索引
	vector<Index_Info> lookup_Indexes(string RelName);

	//通过RID找到数据,返回char*为原件的复制
	char* Find_Record_by_RID(RID rid);

	//插入数据
	void Insert_Reocrd(string RelName, char* record);

	//根据RID删除数据
	void Delete_Record(string RelName, vector<RID> rids);

	//根据RID更新数据
	void Update_Record(string RelName, RID rid, Attr_Info attr, char* new_value);

};

class Reader {  //一个用于遍历数据表的类

public:

	//获取下一条数据，返回char*为原件的复制
	virtual char* get_Next_Record() = 0;

	//获取下一条数据，返回char*为原件的复制，rid赋值为该数据的RID
	virtual char* get_Next_Record_with_RID(RID& rid) = 0;
};

class Scan_Reader :public Reader {

public:

	Scan_Reader(string RelName);

	//获取下一条数据，返回char*为原件的复制
	char* get_Next_Record();

	//获取下一条数据，返回char*为原件的复制，rid赋值为该数据的RID
	char* get_Next_Record_with_RID(RID& rid);
};

class Index_Reader :public Reader {

public:

	//MIN/MAX==NULL代表不受限
	Index_Reader(string RelName, Index_Info Index, char* MIN, char* MAX);

	//获取下一条数据，返回char*为原件的复制
	char* get_Next_Record();

	//获取下一条数据，返回char*为原件的复制，rid赋值为该数据的RID
	char* get_Next_Record_with_RID(RID& rid);

};