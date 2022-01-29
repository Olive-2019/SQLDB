#pragma once
#include "Link_Order_Affirmant.h"
#include "Logical_Tree_Builder.h"
#include "estimator.h"
class DP_Link_Order_Affirmant
	:public Link_Order_Affirmant
{
private:
	vector<Rel_Info> Rels;
	vector<Condition> Conds;

	vector<string> rels_name;

	//下面两个构造函数构造
	//所有表的二元条件映射
	map<vector<string>, Logical_TreeNode*> relation_to_binary_condition_node_map;
	//叶子结点加上一元条件+只有一个表的二元属性
	map<string, Logical_TreeNode*> relation_to_node_map;

	
	map<vector<string>, Logical_TreeNode*> dp_relations_to_node_map;
	map < vector<string>, double> dp_relations_lost;

	//连接好的关系
	vector < vector<vector<string>>> joinrels;

	//连接两个结点
	Logical_TreeNode* join(vector<string> left, vector<string> right);

	//检查是否存在二元条件，如果存在则返回新的结点，如果不存在则返回原来的node
	Logical_TreeNode* add_node_to_binary_condition_node(Logical_TreeNode* node, string rel1, string rel2);

	//获取当前级别的所缺的其它表集合
	vector<vector<string>> get_other_rel(int level, vector<string> cur);
	//产生level级别的最佳连接结果
	void dp_one_level(int level);
    //整体的dp
	void dp() {
		for (int level = 2; level <= joinrels[1].size(); ++level)
			dp_one_level(level);
	}

	void init();

public:
	DP_Link_Order_Affirmant(vector<Rel_Info>& Rels, vector<Condition>& Conds);

	
	Logical_TreeNode* get_tree(){
		init();
		dp();
		return dp_relations_to_node_map[joinrels[joinrels.size() - 1][0]];
	}
};