#include "DDL_executor.h"
#include "../Subsystem1.h"
DDL_executor DDL_executor::ddl_executor;

void DDL_executor::create_table(string tableName, vector<AttrType> types, vector<string> attrNames)
{
	vector<Attr_Info> attrs;
	Attr_Info attr;
	int offset = 0;
	for (int i = 0; i < types.size(); i++) {
		memcpy(attr.Attr_Name, attrNames[i].c_str(), attrNames[i].size() + 1);
		memcpy(attr.Creator, Global_Paras::Current_User.c_str(), Global_Paras::Current_User.size() + 1);
		memcpy(attr.DBName, Global_Paras::Current_DB.c_str(), Global_Paras::Current_DB.size() + 1);
		memcpy(attr.Rel_Name, tableName.c_str(), tableName.size() + 1);
		attr.type = types[i];
		attr.Offset = offset;
		attr.Num_of_Change_Records = 0;
		/*
		警告
		distribution未定
		
		*/
		
		switch (attr.type) {
		case AttrType::INT: {
			offset += sizeof(int);
			attr.Length = sizeof(int);
			break;
		}
		case AttrType::FLOAT: {
			offset += sizeof(float);
			attr.Length = sizeof(float);
			break;
		}
		case AttrType::STRING: {
			offset += 20;
			attr.Length = 20;
			break;
		}
		}
		strcpy(attr.DBName, Subsystem1_Manager::mgr.DBName.c_str());
		strcpy(attr.Creator, Subsystem1_Manager::mgr.UserName.c_str());
		attrs.push_back(attr);
	}

	Subsystem1_Manager::mgr.Create_Rel(tableName, attrs);
}

void DDL_executor::create_DB(string DBName)
{
	
	/*
	警告该接口未定义
	*/
}

void DDL_executor::set_DB(string DBName)
{
	Global_Paras::Current_DB = DBName;
}

void DDL_executor::create_index(string relName, string attrName)
{
	/*
	警告该接口未定义
	*/
}
