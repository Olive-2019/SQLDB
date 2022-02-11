#include "executor.h"
#include "Query_Executor.h"
#include "../Subsystem1.h"
#include <iostream>
#include <iomanip>
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
	Display(RelName, records);
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
	cout << "查询结果为：" << endl;
	vector<Attr_Info> attrs = Subsystem1_Manager::mgr.lookup_Attrs(RelName);
	int length = 15;
	for (int i = 0; i < attrs.size(); ++i) {
		cout << setw(length) << attrs[i].Attr_Name;
	}
	cout << endl;
	for (int i = 0; i < records.size(); ++i) {
		char* buff = Subsystem1_Manager::mgr.Find_Record_by_RID(RelName, records[i]);
		for (int j = 0; j < attrs.size(); ++j) {
			switch (attrs[j].type)
			{
			case AttrType::INT: {
				int data = *(int*)(buff + attrs[j].Offset);
				cout << setw(length) << data;
				break;
			}
			case AttrType::FLOAT: {
				float data = *(float*)(buff + attrs[j].Offset);
				cout << setw(length) << data;
				break;
			}
			case AttrType::STRING: {
				string data = buff + attrs[j].Offset;
				cout << setw(length) << data;
				break;
			}
			}
		}
		cout << endl;
	}
}

void Executor::Insert(string RelName, char* record)
{
	Subsystem1_Manager::mgr.Insert_Reocrd(RelName, record);
}

void Executor::Delete(string RelName, vector<RID> records)
{
	Subsystem1_Manager::mgr.Delete_Record(RelName, records);
}

void Executor::Update(string RelName, vector<RID> records, vector<Attr_Info> attrs, char** new_values)
{
	for (int i = 0; i < records.size(); ++i) 
		for (int j = 0; j < attrs.size(); ++j) 
			Subsystem1_Manager::mgr.Update_Record(RelName, records[i], attrs[j], new_values[j]);
}

void Executor::exam_distribution(string RelName)
{
	vector<Attr_Info> attrs = Subsystem1_Manager::mgr.lookup_Attrs(RelName);
	for (int i = 0; i < attrs.size(); i++) {
		if (attrs[i].Num_of_Change_Records > 10) {
			update_distribution(attrs[i]);
		}
	}
}

void Executor::update_distribution(Attr_Info attr)
{
	Distribution distribution;
	Scan_Reader* reader = new Scan_Reader(attr.Rel_Name);
	distribution.dis = new NORMAL_dis();
	NORMAL_dis* dis = (NORMAL_dis*)distribution.dis;
	dis->mu = reader->get_mu(attr);
	dis->sigma = reader->get_sigma(attr);
	distribution.type = NORMAL;
	Subsystem1_Manager::mgr.set_distribution(attr, distribution);
	Subsystem1_Manager::mgr.set_change_records(attr, 0);
}
