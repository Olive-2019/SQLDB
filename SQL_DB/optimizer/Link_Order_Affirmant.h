#pragma once

#include "../parser/parser_interp.h"
#include "Logical_Tree_Builder.h"
class Link_Order_Affirmant {
private:
	void init();
protected:
	//关系、条件、关系名字
	vector<Rel_Info>& Rels;
	vector<Condition>& Conds;
	vector<string> rels_name;

	//所有表的二元条件映射
	map<vector<string>, Logical_TreeNode*> relation_to_binary_condition_node_map;
	//叶子结点加上一元条件+只有一个表的二元属性
	map<string, Logical_TreeNode*> relation_to_node_map;
	
	//属性
	vector<Attr_Info> Attrs;
public:

	Link_Order_Affirmant(vector<Rel_Info>& Rels, vector<Condition>& Conds);

	/*virtual vector<Condition> get_Link_Order();*/
	virtual Logical_TreeNode* get_tree() = 0;

};