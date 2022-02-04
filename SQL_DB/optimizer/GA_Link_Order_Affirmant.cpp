#include "GA_Link_Order_Affirmant.h"
#include "estimator.h"
#include<ctime>

GA_Link_Order_Affirmant::GA_Link_Order_Affirmant(vector<Rel_Info>& Rels,
	vector<Condition>& Conds, int agent_num, int max_iteration_num)
	:Link_Order_Affirmant(Rels, Conds),
	tree_builder(relation_to_binary_condition_node_map, relation_to_node_map),
	agent_num(agent_num), max_iteration_num(max_iteration_num)
{}

void GA_Link_Order_Affirmant::GA() {
	random_init();
	for (int iter = 0; iter < max_iteration_num; ++iter) {
		vector<int> parent = get_parent();
		vector<int> new_one = crossover(parent);
		new_one = mutate(new_one);
		Logical_TreeNode* node = get_tree_with_order(new_one);
		double new_one_loss = Estimator(node).estimate();
		double max_loss = new_one_loss;
		int max_index = -1;
		for (int i = 0; i < agent_num; ++i) if (loss[i] > max_loss) max_loss = loss[i], max_index = i;
		//不能替换的情况
		if (max_index < 0) {
			tree_builder.delete_node(node);
			continue;
		}
		//需要替换的情况
		loss[max_index] = max_loss;
		agents[max_index] = new_one;
		tree_builder.delete_node(agents_tree[max_index]);
		agents_tree[max_index] = node;

	}
}
void GA_Link_Order_Affirmant::random_init() {
	int dimension_num = rels_name.size();
	int* seed = new int[dimension_num]();
	for (int i = 0; i < dimension_num; ++i) seed[i] = i;
	for (int i = 0; i < agent_num; ++i) {
		srand(unsigned(time(0)));
		random_shuffle(seed, seed + dimension_num);
		vector<int> temp(seed, seed + dimension_num);
		agents.push_back(temp);
		agents_tree.push_back(get_tree_with_order(agents[i]));
		loss.push_back(Estimator(agents_tree[i]).estimate());
	}
}
vector<int> GA_Link_Order_Affirmant::get_parent() {
	vector<int> tmp = { 0, 1 };
	return tmp;
}

vector<int> GA_Link_Order_Affirmant::crossover(vector<int> paraent) {
	return agents[paraent[0]];
}

vector<int> GA_Link_Order_Affirmant::mutate(vector<int> old) {
	return old;
}
Logical_TreeNode* GA_Link_Order_Affirmant::get_tree_with_order(vector<int> order) {
	vector<string> name;
	for (int i = 0; i < order.size(); ++i) 
		name.push_back(rels_name[i]);
	return tree_builder.get_tree_root_with_order(name);
}

Logical_TreeNode* GA_Link_Order_Affirmant::get_tree() {
	GA();
	int index = -1;
	double min_loss = 0x7FFFFFFF;
	for (int i = 0; i < agent_num; ++i)
		if (loss[i] < min_loss) min_loss = loss[i], index = i;
	for (int i = 0; i < agent_num; ++i)
		if (i != index) tree_builder.delete_node(agents_tree[i]);
	return agents_tree[index];
}