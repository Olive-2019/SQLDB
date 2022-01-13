#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
using namespace std;

const int MAX_NAME_LENGTH = 20;    //各种名字如数据表名、字段名等最大长度



enum Distribution_Type {
	//正态分布
	NORMAL
	//均匀分布
	,EVENLY
};

struct Distribution {
	Distribution_Type type;
	union {
		struct NORMAL {
			int mu;
			int sigma;
		};
		struct EVENLY {
			int MIN;
			int MAX;
		};
	};
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
	SELECT,INSERT,DELETE,UPDATE
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
