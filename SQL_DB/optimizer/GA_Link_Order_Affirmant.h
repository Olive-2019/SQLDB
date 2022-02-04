#include "Link_Order_Affirmant.h"

class GA_Link_Order_Affirmant : public Link_Order_Affirmant {
	
	const int agent_num, max_iteration_num;
	
	Logical_Tree_Builder tree_builder;
	//粒子位置
	vector<vector<int>> agents;
	//粒子对应的树
	vector<Logical_TreeNode*> agents_tree;
	//粒子损失
	vector<double> loss;

	void GA();
	
	void random_init();

	//获取两个父亲agent
	vector<int> get_parent();

	//将两个父亲agent杂交，获得儿子agent
	vector<int> crossover(vector<int> paraent);

	//对agent进行变异
	vector<int> mutate(vector<int> old);

	//根据order获取逻辑树
	Logical_TreeNode* get_tree_with_order(vector<int> order);
	
public:
	GA_Link_Order_Affirmant(vector<Rel_Info>& Rels, vector<Condition>& Conds, int agent_num = 10, int max_iteration_num = 30);

	Logical_TreeNode* get_tree();
};