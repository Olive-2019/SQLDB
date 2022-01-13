#pragma once
#include "../optimizer/optimizer.h"
#include "../RECORD.h"
#include "../optimizer/Logical_Tree_Builder.h"
class Query_Executor {

private:
	Logical_TreeNode* Root;

public:
	Query_Executor(Logical_TreeNode* Root);
	//ִ�в�ѯ
	vector<RID> query();
	//�Ծ�������е�execute
	vector<RID> execute_tree_node(Logical_TreeNode* node);
};