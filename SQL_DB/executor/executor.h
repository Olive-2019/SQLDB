#pragma once

#include "../RECORD.h"
#include "../optimizer/Logical_Tree_Builder.h"
#include "../Subsystem1.h"
class Executor {
public:
	Executor(Logical_TreeNode* Root);
	void execute_select();
	void execute_update(vector<Attr_Info> attrs, char** new_values);
	void execute_delete();
	void execute_insert(string RelName, char* record);
	void execute();
	void show_distribution(char* relname, char* attrname);
	void execute_script(char* path);
private:
	Logical_TreeNode* Root;
	void Display(string RelName, vector<RID> records);
	void Insert(string RelName, char* record);
	void Delete(string RelName, vector<RID> records);
	void Update(string RelName, vector<RID> records, vector<Attr_Info> attrs, char** new_values);
	void exam_distribution(string RelName);
	void update_distribution(Attr_Info attr);
	
};