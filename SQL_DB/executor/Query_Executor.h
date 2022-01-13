#pragma once
#include "../optimizer/optimizer.h"
#include "../RECORD.h"
#include "../optimizer/Logical_Tree_Builder.h"
class Query_Executor {

private:
	Logical_TreeNode* Root;

public:
	Query_Executor(Logical_TreeNode* Root);
	//执行查询
	vector<RID> query();
	//对具体结点进行的execute
	vector<RID> execute_tree_node(Logical_TreeNode* node);
};