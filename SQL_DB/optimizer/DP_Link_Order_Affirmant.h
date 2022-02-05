#pragma once
#include "Link_Order_Affirmant.h"

#include "estimator.h"
class DP_Link_Order_Affirmant
	:public Link_Order_Affirmant
{
private:
	map<vector<string>, Logical_TreeNode*> dp_relations_to_node_map;
	map < vector<string>, double> dp_relations_lost;

	//���ӺõĹ�ϵ
	vector <vector<vector<string>>> joinrels;

	//�����������
	Logical_TreeNode* join(vector<string> left, vector<string> right);

	//����Ƿ���ڶ�Ԫ��������������򷵻��µĽ�㣬����������򷵻�ԭ����node
	Logical_TreeNode* add_node_to_binary_condition_node(Logical_TreeNode* node, string rel1, string rel2);

	//��ȡ��ǰ�������ȱ����������
	vector<vector<string>> get_other_rel(int level, vector<string> cur);

	//����level�����������ӽ��
	void dp_one_level(int level);

    //�����dp
	void dp() {
		for (int level = 2; level <= joinrels[1].size(); ++level)
			dp_one_level(level);
	}

	

public:
	DP_Link_Order_Affirmant(vector<Rel_Info>& Rels, vector<Condition>& Conds, vector<Attr_Info>& Attrs);

	//��ȡdp�㷨�Ľ���߼���
	Logical_TreeNode* get_tree(){
		dp();
		return add_proj(dp_relations_to_node_map[joinrels[joinrels.size() - 1][0]]);
	}
};