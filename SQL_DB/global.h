#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <random>

using namespace std;

const int MAX_NAME_LENGTH = 20;    //各种名字如数据表名、字段名等最大长度

enum SQL_type {
	SELECT,
	UPDATE,
	DELETE,
	INSERT
};

enum Distribution_Type {
	//正态分布
	NORMAL
	//均匀分布
	,EVENLY
};
struct Distribution;
struct DISTRIBUTION_TYPE {
	//一元条件
	virtual double rate(int op, double value) = 0;
	//二元条件
	virtual double binary_rate(int op, const Distribution& dis) = 0;
};
struct NORMAL : public DISTRIBUTION_TYPE {
	double mu;
	double sigma;
	double rate(int op, double value) {
		return 0.5;
	}
	double binary_rate(int op, const Distribution& dis) {
		return 0.5;
	}
};
struct EVENLY : public DISTRIBUTION_TYPE {
	double MIN;
	double MAX;
	double rate(int op, double value);
	double binary_rate(int op, const Distribution& dis) {
		return 0.5;
	}
};
struct Distribution {
	Distribution_Type type;
	DISTRIBUTION_TYPE* dis;
};


class Global_Paras {  //用于存放一些全局变量，如当前数据库、用户等，不可实例化
private:
	Global_Paras();

public:
	static string Current_DB;
	static string Current_User;

	static int Block_Size;	
};



struct RID {
	int blockID;
	int slotID;
};


enum AttrType {
	INT,FLOAT,STRING
};
enum Authority {
	Authority_SELECT, Authority_INSERT, Authority_DELETE, Authority_UPDATE
};
enum IndexType {
	CLUSTER_INDEX,NORMAL_INDEX
};


struct DB_Info {
	char DBName[MAX_NAME_LENGTH];
	char Creator[MAX_NAME_LENGTH];
};
struct User_Info {
	char UserName[MAX_NAME_LENGTH];
	char PassWord[MAX_NAME_LENGTH];
};
struct Rel_Info {
	char Rel_Name[MAX_NAME_LENGTH];
	char Creator[MAX_NAME_LENGTH];
	char DBName[MAX_NAME_LENGTH];
	int Record_Num;
	int PageID_Head;
};
struct Authority_Info {
	char UserName[MAX_NAME_LENGTH];
	char Rel_Name[MAX_NAME_LENGTH];
	char Creator[MAX_NAME_LENGTH];
	char DBName[MAX_NAME_LENGTH];
	bool authority[4];      //authority[SELECT]=true/false
};
struct Page_Info {
	char Rel_Name[MAX_NAME_LENGTH];
	char Creator[MAX_NAME_LENGTH];
	char DBName[MAX_NAME_LENGTH];
	int PageID;
	int NextID;
};
struct Attr_Info {
	char Rel_Name[MAX_NAME_LENGTH];
	char Creator[MAX_NAME_LENGTH];
	char DBName[MAX_NAME_LENGTH];
	char Attr_Name[MAX_NAME_LENGTH];
	AttrType type;
	int Length;
	int Offset;
	Distribution distribution;
	int Num_of_Change_Records;
};
struct Index_Info {
	char Rel_Name[MAX_NAME_LENGTH];
	char Creator[MAX_NAME_LENGTH];
	char DBName[MAX_NAME_LENGTH];
	char Attr_Name[MAX_NAME_LENGTH];
	char Index_Creator[MAX_NAME_LENGTH];
	int Head_PageID;
	IndexType type;
};
