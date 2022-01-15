#pragma once
#include "../optimizer/optimizer.h"
#include "../RECORD.h"
#include "../optimizer/Logical_Tree_Builder.h"
class Query_Executor {

private:
	Logical_TreeNode* Root;
	//��¼�������ĶԱ�
	bool record_cmp(Condition* cond, char* record, vector<Attr_Info> attrs);
	bool record_cmp(Condition* cond, RID record, vector<Attr_Info> attrs);
public:
	Query_Executor(Logical_TreeNode* Root);
	//ִ�в�ѯ
	vector<RID> query();
	//�Ծ�������е�execute
	vector<RID> execute_tree_node(Logical_TreeNode* node, vector<void*>& cond, vector<Attr_Info>& attrs);
};