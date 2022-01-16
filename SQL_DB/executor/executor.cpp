#include "executor.h"
#include "Query_Executor.h"

Executor::Executor(Logical_TreeNode* Root)
{
	this->Root = Root;
}

void Executor::execute_select()
{
	Query_Executor* query_executor = new Query_Executor(Root);
	vector<RID> records = query_executor->query();
	string RelName = query_executor->get_final_RelName();
	Display(RelName, records);
}

void Executor::execute_update(vector<Attr_Info> attrs, char** new_values)
{
	Query_Executor* query_executor = new Query_Executor(Root);
	vector<RID> records = query_executor->query();
	string RelName = query_executor->get_final_RelName();
	Update(RelName, records, attrs, new_values);
}

void Executor::execute_delete()
{
	Query_Executor* query_executor = new Query_Executor(Root);
	vector<RID> records = query_executor->query();
	string RelName = query_executor->get_final_RelName();
	Delete(RelName, records);
}

void Executor::execute_insert(string RelName, char* record)
{
	Insert(RelName, record);
}

void Executor::execute()
{

}

void Executor::Display(string RelName, vector<RID> records)
{
}

void Executor::Insert(string RelName, char* record)
{
}

void Executor::Delete(string RelName, vector<RID> records)
{
}

void Executor::Update(string RelName, vector<RID> records, vector<Attr_Info> attrs, char** new_values)
{
}
