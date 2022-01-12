#pragma once
#include "global.h"
#include "parser/parser_interp.h"
class Subsystem1_Manager {
private:
	Subsystem1_Manager() {

	}
public:
	static Subsystem1_Manager BASE;

	//查找数据表，返回值为是否成功，将结果存放之最后的参数rel中，后面的函数同理
	bool lookup_Rel(string RelName, Rel_Info& rel);

	//查找属性
	bool lookup_Attr(string RelName, string AttrName, Attr_Info& attr);

	//查找所有属性
	vector<Attr_Info> lookup_Attrs(string RelName);

	//查询权限 ret[SELECT]=true,ret[UPDATE]=false，数据表或用户不存在返回NULL
	bool* lookup_Authority(string RelName, string UserName);

	//查找索引
	bool lookup_Index(string RelName, string AttrName, Index_Info& Index);

	//查找所有索引
	vector<Index_Info> lookup_Indexes(string RelName);

	//通过RID找到数据,返回char*为原件的复制，不存在返回NULL
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
private:
	vector<Condition> Conds;
	//过滤器
	bool filter(char* record);
public:
	Scan_Reader(string RelName, vector<Condition> Conds);
	//获取下一条数据，返回char*为原件的复制，返回值必须满足filter
	char* get_Next_Record();
	//获取下一条数据，返回char*为原件的复制，rid赋值为该数据的RID
	char* get_Next_Record_with_RID(RID& rid);
};

class Index_Reader :public Reader {
public:
	//MIN/MAX==NULL代表不受限，MIN_E与MAX_E代表能否取等
	Index_Reader(string RelName, Index_Info Index, bool MIN_E, char* MIN, bool MAX_E, char* MAX);
	//获取下一条数据，返回char*为原件的复制
	char* get_Next_Record();
	//获取下一条数据，返回char*为原件的复制，rid赋值为该数据的RID
	char* get_Next_Record_with_RID(RID& rid);
};