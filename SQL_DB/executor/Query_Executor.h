#pragma once
#include "../optimizer/optimizer.h"
#include "../RECORD.h"
#include "../optimizer/Logical_Tree_Builder.h"
class Query_Executor {

private:
	Logical_TreeNode* Root;
	//记录和条件的对比
	bool record_cmp(Condition* cond, char* record, string RelName);
	bool record_cmp(Condition* cond, RID rid, string RelName);
	//一元条件cmp
	bool record_cmp_1(Condition* cond, char* record, string RelName);
	//二元条件cmp
	bool record_cmp_2(Condition* cond, char* record, string RelName);
	//二元条件比较函数，0相等，1大于，-1小于
	int cmp(char* p1, char* p2, Attr_Info Lattr, Attr_Info Rattr);
	//一元条件比较函数
	int cmp(char* p1, Value p2, Attr_Info Lattr);
	float get_value(char* value, AttrType type);
public:
	Query_Executor(Logical_TreeNode* Root);
	//执行查询
	vector<RID> query();
	//对具体结点进行的execute
	vector<RID> execute_tree_node(Logical_TreeNode* node);
	//对叶节点的执行，conds为一元条件
	vector<RID> execute_tree_node_scan(Logical_TreeNode* node, vector<Condition*> cond = vector<Condition*>());
	//对于所有的filter调用的
	vector<RID> execute_tree_node_filter(Logical_TreeNode* node);
	//一元的filter，一元条件下沉
	vector<RID> execute_tree_node_under_filter(Logical_TreeNode* node, vector<Condition*>& cond);
	//join，对二元条件下沉
	vector<RID> execute_tree_node_join(Logical_TreeNode* node, vector<Condition*> cond = vector<Condition*>());
	//投影结点的处理
	vector<RID> execute_tree_node_proj(Logical_TreeNode* node);
};