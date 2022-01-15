#pragma once
#include "../optimizer/optimizer.h"
#include "../RECORD.h"
#include "../optimizer/Logical_Tree_Builder.h"
class Query_Executor {

private:
	Logical_TreeNode* Root;
	//记录和条件的对比
	bool record_cmp(Condition* cond, char* record, vector<Attr_Info> attrs);
	bool record_cmp(Condition* cond, RID record, vector<Attr_Info> attrs);
public:
	Query_Executor(Logical_TreeNode* Root);
	//执行查询
	vector<RID> query();
	//对具体结点进行的execute
	vector<RID> execute_tree_node(Logical_TreeNode* node);
	//对叶节点的执行，conds为一元条件
	vector<RID> execute_tree_node_scan(Logical_TreeNode* node, vector<Condition*> cond = vector<Condition*>());
	vector<RID> execute_tree_node_filter(Logical_TreeNode* node);
	vector<RID> execute_tree_node_under_filter(Logical_TreeNode* node, vector<Condition*> cond);
	vector<RID> execute_tree_node_join(Logical_TreeNode* node, vector<Condition*> cond = vector<Condition*>());
	vector<RID> execute_tree_node_proj(Logical_TreeNode* node);


};