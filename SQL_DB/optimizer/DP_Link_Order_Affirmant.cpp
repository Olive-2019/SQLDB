#include "DP_Link_Order_Affirmant.h"


//������
vector<vector<string>> zFun(const std::vector<string>& a, int m)
{
	std::vector<std::vector<string>> result;

	// �ݹ��������
	if (m == 1)
	{
		for (int i = 0; i < a.size(); ++i)
		{
			result.push_back(std::vector<string>({ a[i] }));
		}
		return result;
	}

	// һ�����
	for (int i = 0; i < a.size(); ++i)
	{
		// ��ȥ��һ��Ԫ��֮���ʣ��Ԫ�ؽ���ѡȡ(m-1)��
		std::vector<string> b = a;
		std::vector<string>::iterator it = b.begin();
		b.erase(it, it + i + 1); // ����Ҫע��������в�����˳��ǰ���Ѿ����ֹ��ģ�����Ͳ�Ҫ�����ˣ�12�Ѿ����ǹ��ˣ���ô21�Ͳ��ÿ�����
		std::vector<std::vector<string>> ret = zFun(b, m - 1);
		// ��һ��Ԫ��+ʣ�µ�Ԫ�ؽ�����װ
		for (int j = 0; j < ret.size(); ++j)
		{
			std::vector<string> one;
			one.push_back(a[i]);
			for (int k = 0; k < ret[j].size(); ++k)
			{
				one.push_back(ret[j][k]);
			}
			result.push_back(one);
		}
	}
	return result;
}





DP_Link_Order_Affirmant::DP_Link_Order_Affirmant(vector<Rel_Info>& Rels, vector<Condition>& Conds, vector<Attr_Info>& Attrs)
	: Link_Order_Affirmant(Rels, Conds, Attrs) {
	//joinrels��һ��ռλ
	joinrels.push_back(vector<vector<string>>());
	//�ڶ����������еı�
	vector < vector<string>> rels;
	for (int i = 0; i < rels_name.size(); ++i) {
		vector<string> tmp = { rels_name[i] };
		dp_relations_to_node_map[tmp] = relation_to_node_map[rels_name[i]];
		rels.push_back(tmp);
	}
	joinrels.push_back(rels);
	for (int i = 2; i <= rels_name.size(); ++i) joinrels.push_back(vector<vector<string>>());
	
	
}
Logical_TreeNode* DP_Link_Order_Affirmant::add_node_to_binary_condition_node(Logical_TreeNode* node, string rel1, string rel2) {
	vector<string> rels = { rel1, rel2 };
	sort(rels.begin(), rels.end());
	if (!relation_to_binary_condition_node_map.count(rels)) return node;
	Logical_TreeNode* p = relation_to_binary_condition_node_map[rels], * p0 = NULL;
	Logical_TreeNode* new_binary_tree, *root(NULL), *rear(NULL);
	while (p) {
		new_binary_tree = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILTER);
		new_binary_tree->u.FILTER.rel = NULL;
		memcpy(new_binary_tree, p, sizeof(*p));
		if (p == relation_to_binary_condition_node_map[rels]) {
			rear = root = new_binary_tree;
		}
		else {
			rear->u.FILTER.rel = new_binary_tree;
			rear = new_binary_tree;
		}
		p0 = p, p = p->u.FILTER.rel;
	} 
	rear->u.FILTER.rel = node;
	return root;
}
vector<vector<string>> DP_Link_Order_Affirmant::get_other_rel(int level, vector<string> cur) {
	int other_num = level - cur.size();
	vector<string> result;
	set_difference(rels_name.begin(), rels_name.end(), cur.begin(), cur.end(), inserter(result, result.begin()));
	//��˳�򷵻�
	return zFun(result, other_num);
}
Logical_TreeNode* DP_Link_Order_Affirmant::join(vector<string> left, vector<string> right) {
	Logical_TreeNode* LNode = dp_relations_to_node_map[left],
		* RNode = dp_relations_to_node_map[right];
	Logical_TreeNode* filter_node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_JOIN);
	filter_node->u.JOIN.left = LNode;
	filter_node->u.JOIN.right = RNode;
	Logical_TreeNode* ans = filter_node;
	for (int left_i = 0; left_i < left.size(); ++left_i) {
		for (int right_i = 0; right_i < right.size(); ++right_i) {
			ans = add_node_to_binary_condition_node(ans, left[left_i], right[right_i]);
		}
	}
	return ans;
}
void DP_Link_Order_Affirmant::dp_one_level(int level) {
	
	for (int i = 0; i < joinrels[level - 1].size(); ++i) {
		//level==2ʱ��level-1==1
		Logical_TreeNode* node = dp_relations_to_node_map[joinrels[level - 1][i]];
		
		if (level != 2 || true) {

			vector<vector<string>> other_rels = get_other_rel(level, joinrels[level - 1][i]);
			for (int other_rel_index = 0; other_rel_index < other_rels.size(); ++other_rel_index) {
				
				
				vector<string> new_rel;
				Logical_TreeNode* node = join(joinrels[level - 1][i], other_rels[other_rel_index]);
				set_union(joinrels[level - 1][i].begin(), joinrels[level - 1][i].end(),
					other_rels[other_rel_index].begin(), other_rels[other_rel_index].end(),
					inserter(new_rel, new_rel.begin()));
				sort(new_rel.begin(), new_rel.end());
				double link_lost = Estimator(node).estimate();
				if (dp_relations_to_node_map.count(new_rel)) {
					if (dp_relations_lost[new_rel] > link_lost) {
						dp_relations_lost[new_rel] = link_lost;
						dp_relations_to_node_map[new_rel] = node;
					}
				}
				else {
					dp_relations_lost[new_rel] = link_lost;
					dp_relations_to_node_map[new_rel] = node;
					joinrels[level].push_back(new_rel);
				}
			}
		}
	}
	for (int k = 2;; k++)
	{
		int other_level = level - k;

		if (k > other_level)
			break;

		for (int i = 0; i < joinrels[k].size(); ++i) {
			vector<vector<string>> other_rels = get_other_rel(level, joinrels[k][i]);
			for (int other_rel_index = 0; other_rel_index < other_rels.size(); ++other_rel_index) {
				vector<string> new_rel;
				Logical_TreeNode* node = join(joinrels[k][i], other_rels[other_rel_index]);
				set_union(joinrels[k][i].begin(), joinrels[k][i].end(),
					other_rels[other_rel_index].begin(), other_rels[other_rel_index].end(),
					inserter(new_rel, new_rel.begin()));
				sort(new_rel.begin(), new_rel.end());
				double link_lost = Estimator(node).estimate();
				if (dp_relations_to_node_map.count(new_rel)) {
					if (dp_relations_lost[new_rel] > link_lost) {
						dp_relations_lost[new_rel] = link_lost;
						dp_relations_to_node_map[new_rel] = node;
					}
				}
				else {
					dp_relations_lost[new_rel] = link_lost;
					dp_relations_to_node_map[new_rel] = node;
					joinrels[level].push_back(new_rel);
				}
			}
		}
	}
}