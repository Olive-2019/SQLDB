#include "GA_Link_Order_Affirmant.h"
#include "estimator.h"
#include<ctime>

GA_Link_Order_Affirmant::GA_Link_Order_Affirmant(vector<Rel_Info>& Rels,
	vector<Condition>& Conds, vector<Attr_Info>& Attrs, int agent_num, int max_iteration_num)
	:Link_Order_Affirmant(Rels, Conds, Attrs),
	agent_num(agent_num), max_iteration_num(max_iteration_num)
{}

void GA_Link_Order_Affirmant::GA() {
	random_init();
	for (int iter = 0; iter < max_iteration_num; ++iter) {
		vector<int> parent = get_parent();
		vector<int> new_one = crossover(parent);
		mutate(new_one);
		
		Logical_TreeNode* node = get_tree_with_order(new_one);
		double new_one_loss = Estimator(node).estimate();
		double max_loss = new_one_loss;
		int max_index = -1;
		for (int i = 0; i < agent_num; ++i) if (loss[i] > max_loss) max_loss = loss[i], max_index = i;
		//不能替换的情况
		if (max_index < 0) {
			tree_builder->delete_node(node);
			continue;
		}
		//需要替换的情况
		loss[max_index] = max_loss;
		agents[max_index] = new_one;
		tree_builder->delete_node(agents_tree[max_index]);
		agents_tree[max_index] = node;
	}
}
void GA_Link_Order_Affirmant::random_init() {
	dimension_num = rels_name.size();
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
	vector<double> loss_tmp = loss;
	double sum = 0;
	double min = 0x7FFFFFFFFFFFFFFF;
	for (int i = 0; i < agent_num; ++i)
		if (loss_tmp[i] < min && min) min = loss_tmp[i];
	for (int i = 0; i < agent_num; ++i) {
		if (loss_tmp[i]) loss_tmp[i] = 1 / loss_tmp[i];
		else loss_tmp[i] = 1 / min;
		sum += loss_tmp[i];
	}
	for (int i = 0; i < agent_num; ++i) loss_tmp[i] /= sum, loss_tmp[i] *= 100;
	for (int i = 1; i < agent_num; ++i) loss_tmp[i] += loss_tmp[i - 1];
	vector<int> ans;
	
	srand(unsigned(time(0)));
	int r = rand() % 100;
	//找到第一个
	for (int i = 0; i < agent_num; ++i) 
		if (r < loss_tmp[i]) {
			ans.push_back(i);
			break;
		}
	bool stop = false;
	while (!stop) {
		int r = rand() % 100;
		for (int i = 0; i < agent_num; ++i)
			if (r < loss_tmp[i] && i != ans[0]) {
				ans.push_back(i);
				stop = true;
				break;
			}
	}
	return ans;
}

vector<int> GA_Link_Order_Affirmant::crossover(vector<int> parent) {
	vector<vector<int>> edge;
	vector<int> new_one;
	vector<int> tmp(dimension_num, 0);
	for (int i = 0; i < dimension_num; ++i) edge.push_back(tmp);
	//初始化边表
	for (int i = 0; i < parent.size(); ++i) {
		int index = parent[i];
		for (int j = 0; j < dimension_num; ++j) {
			edge[j][agents[index][(j + 1) % dimension_num]]++;
			edge[j][agents[index][(j - 1 + dimension_num) % dimension_num]]++;
		}
	}
	vector<vector<int>> share_edge, exclusive_edge;
	for (int i = 0; i < edge.size(); ++i) {
		vector<int> tmp;
		share_edge.push_back(tmp);
		exclusive_edge.push_back(tmp);
		for (int j = 0; j < dimension_num; ++j) {
			switch (edge[i][j])
			{
			case 1:
				exclusive_edge[i].push_back(j);
				break;
			case 2:
				share_edge[i].push_back(j);
				break;
			}
		}
	}
	//把第一个随机出来
	srand(unsigned(time(0)));
	new_one.push_back(rand() % dimension_num);
	//删除边
	for (int j = 0; j < dimension_num; ++j) {
		vector<int>::iterator it = remove(share_edge[j].begin(), share_edge[j].end(), new_one[0]);
		share_edge[j].erase(it, share_edge[j].end());
		it = remove(exclusive_edge[j].begin(), exclusive_edge[j].end(), new_one[0]);
		exclusive_edge[j].erase(it, exclusive_edge[j].end());
	}

	//选择边
	for (int i = 1; i < dimension_num; ++i) {
		int front = new_one[i - 1];
		int ans = -1;
		//优先考虑共享边,其次考虑连接边，如果都没有，就随机
		if (share_edge[front].size()) {
			random_shuffle(share_edge[front].begin(), share_edge[front].end());
			ans = share_edge[front][0];
		}
		else if (exclusive_edge[front].size()){
			random_shuffle(exclusive_edge[front].begin(), exclusive_edge[front].end());
			ans = exclusive_edge[front][0];
		}
		else {
			for (int j = 0; j < dimension_num; ++j)
				if (!count(new_one.begin(), new_one.end(), j)) {
					ans = j;
					break;
				}
		}
		new_one.push_back(ans);
		//删除边
		for (int j = 0; j < dimension_num; ++j) {
			vector<int>::iterator it = remove(share_edge[j].begin(), share_edge[j].end(), ans);
			share_edge[j].erase(it, share_edge[j].end());
			it = remove(exclusive_edge[j].begin(), exclusive_edge[j].end(), ans);
			exclusive_edge[j].erase(it, exclusive_edge[j].end());
		}
	}
	return new_one;
}

void GA_Link_Order_Affirmant::mutate(vector<int>& old) {
	for (int i = 0; i < dimension_num; ++i) {
		int first, second;
		srand(unsigned(time(0)));
		first = rand() % dimension_num;
		second = rand() % dimension_num;
		int tmp = old[first];
		old[first] = old[second];
		old[second] = tmp;
	}

}
Logical_TreeNode* GA_Link_Order_Affirmant::get_tree_with_order(vector<int> order) {
	vector<string> name;
	for (int i = 0; i < order.size(); ++i) name.push_back(rels_name[i]);
	return tree_builder->get_tree_root_with_order(name);
}

Logical_TreeNode* GA_Link_Order_Affirmant::get_tree() {
	GA();
	int index = -1;
	double min_loss = 0x7FFFFFFF;
	for (int i = 0; i < agent_num; ++i)
		if (loss[i] < min_loss) min_loss = loss[i], index = i;
	for (int i = 0; i < agent_num; ++i)
		if (i != index) tree_builder->delete_node(agents_tree[i]);
	return add_proj(agents_tree[index]);
}