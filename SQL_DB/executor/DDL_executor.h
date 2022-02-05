#pragma once
#include "../global.h"

class DDL_executor {
public:
	static DDL_executor ddl_executor;

	void create_table(string tableName, vector<AttrType> types, vector<string> attrNames);
	void create_DB(string DBName);
	void set_DB(string DBName);
	void create_index(string relName, string attrName);
};