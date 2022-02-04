#include "Link_Order_Affirmant.h"

class GA_Link_Order_Affirmant : public Link_Order_Affirmant {
	
	const int agent_num, max_iteration_num;
	
	Logical_Tree_Builder tree_builder;
	//����λ��
	vector<vector<int>> agents;
	//���Ӷ�Ӧ����
	vector<Logical_TreeNode*> agents_tree;
	//������ʧ
	vector<double> loss;

	void GA();
	
	void random_init();

	//��ȡ��������agent
	vector<int> get_parent();

	//����������agent�ӽ�����ö���agent
	vector<int> crossover(vector<int> paraent);

	//��agent���б���
	vector<int> mutate(vector<int> old);

	//����order��ȡ�߼���
	Logical_TreeNode* get_tree_with_order(vector<int> order);
	
public:
	GA_Link_Order_Affirmant(vector<Rel_Info>& Rels, vector<Condition>& Conds, int agent_num = 10, int max_iteration_num = 30);

	Logical_TreeNode* get_tree();
};