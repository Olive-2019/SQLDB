#include "Logical_Tree_Builder.h"
#include "../Subsystem1.h"
#include <iostream>
#include <map>
#include <queue>
using namespace std;

Logical_Tree_Builder::Logical_Tree_Builder(vector<Rel_Info> Rels, vector<Attr_Info> Attrs, vector<Condition> Conds)
{
	this->Rels = Rels;
	this->Attrs = Attrs;
	this->Conds = Conds;
}

Logical_TreeNode* Logical_Tree_Builder::get_tree_root()
{
	//Logical_TreeNode* Root;
	//����������
	/*
	����

	��������ѯ�����漰�������ݱ�һ�£������ᵼ�´����Ŀռ��˷ѣ�
	��Ϊ�Ὣ�������Ӻ���±� �����������������

	��δ����
	*/
	//��������

	Root = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_PROJ);
	/*
	�����߼���
	*/

	map<string, Logical_TreeNode*> mp_Rel_to_Node;  //���ݱ������ҽ��
	//˫��������������⣨���֮ǰ�Ҳ�����������ݿⳣ�ü����ˡ��ռ任ʱ�䡿��
	map<Logical_TreeNode*, vector<string>> node_to_rel_map;//���ݽ���ұ���
	for (int i = 0; i < Rels.size(); i++) {   //����Ҷ�ڵ�
		cout << Rels[i].Rel_Name << endl;
		Logical_TreeNode* node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILESCAN);
		node->u.FILESCAN.Rel = Rels[i].Rel_Name;
		mp_Rel_to_Node[Rels[i].Rel_Name] = node;
		vector<string> rel_names = { Rels[i].Rel_Name };
		node_to_rel_map[node] = rel_names;
	}
	for (int i = 0; i < Conds.size(); i++) {  //��������һԪ��������������filter
		if (Conds[i].bRhsIsAttr == false) {
			Rel_Info temp;
			Subsystem1_Manager::mgr.lookup_Rel(Conds[i].lhsAttr.relname, temp);
			cout << "cond RelName==" << temp.Rel_Name << endl;
			Logical_TreeNode* leaf = mp_Rel_to_Node[temp.Rel_Name];
			Logical_TreeNode* node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILTER);
			node->u.FILTER.rel = leaf;
			node->u.FILTER.expr_filter = new Condition;
			memcpy(node->u.FILTER.expr_filter, &(Conds[i]), sizeof(Condition));//����filter����滻ԭ����Ҷ�ڵ�
			node_to_rel_map[node] = node_to_rel_map[leaf];
			node_to_rel_map.erase(node_to_rel_map.find(leaf));
			mp_Rel_to_Node[Conds[i].lhsAttr.relname] = node;
		}
	}

	//�޸ģ������漰����ͬ�Ķ�Ԫ��������ʱ��������join
	
	for (int i = 0; i < Conds.size(); i++) {  //���ݶ�Ԫ������һ�������߼���
		if (Conds[i].bRhsIsAttr == true) {
			Rel_Info Ltemp, Rtemp;
			Subsystem1_Manager::mgr.lookup_Rel(Conds[i].lhsAttr.relname, Ltemp);
			Subsystem1_Manager::mgr.lookup_Rel(Conds[i].rhsAttr.relname, Rtemp);
			Logical_TreeNode* Lnode = mp_Rel_to_Node[Ltemp.Rel_Name];
			Logical_TreeNode* Rnode = mp_Rel_to_Node[Rtemp.Rel_Name];
			
			if (Lnode != Rnode) {
				Logical_TreeNode* node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_JOIN);
				node->u.JOIN.left = Lnode;  //����join����滻ԭ��������Ҷ�ڵ�
				node->u.JOIN.right = Rnode;

				Logical_TreeNode* Filter_Node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILTER); //�������Ӻ��һ��filter����
				Filter_Node->u.FILTER.rel = node;
				
				Filter_Node->u.FILTER.expr_filter = new Condition;
				memcpy(Filter_Node->u.FILTER.expr_filter, &Conds[i], sizeof(Condition));
				//������Ӧ��ϵ�ĺϲ�
				vector<string> LRel = node_to_rel_map[Lnode], RRel = node_to_rel_map[Rnode];
				LRel.insert(LRel.end(), RRel.begin(), RRel.end());
				unique(LRel.begin(), LRel.end());
				node_to_rel_map[Filter_Node] = LRel;
				//ɾ��ԭ����
				node_to_rel_map.erase(node_to_rel_map.find(Lnode));
				node_to_rel_map.erase(node_to_rel_map.find(Rnode));
				//���¶�Ӧ�Ľ��
				for (int Rel_index = 0; Rel_index < LRel.size(); ++Rel_index) 
					mp_Rel_to_Node[LRel[Rel_index]] = Filter_Node;
			}
			else {
				Logical_TreeNode* Filter_Node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILTER); //�������Ӻ��һ��filter����
				Filter_Node->u.FILTER.rel = Lnode;
				Filter_Node->u.FILTER.expr_filter = new Condition;
				memcpy(Filter_Node->u.FILTER.expr_filter, &Conds[i], sizeof(Condition));
				node_to_rel_map[Filter_Node] = node_to_rel_map[Lnode];
				node_to_rel_map.erase(node_to_rel_map.find(Lnode));
				for (int Rel_index = 0; Rel_index < node_to_rel_map[Filter_Node].size(); ++Rel_index)
					mp_Rel_to_Node[node_to_rel_map[Filter_Node][Rel_index]] = Filter_Node;
			}
		}
	}


	{
		//�����������������б���������
		Rel_Info temp = Rels[0];
		Logical_TreeNode* Final = mp_Rel_to_Node[temp.Rel_Name];
		map<Logical_TreeNode*, bool> mp_is_node_integrated_before;//�ý���Ƿ��ѱ�����
		mp_is_node_integrated_before[Final] = true;
		for (int i = 1; i < Rels.size(); i++) {
			Logical_TreeNode* node = mp_Rel_to_Node[Rels[i].Rel_Name];
			if (mp_is_node_integrated_before.count(node)) {
				continue;
			}
			vector<string> rel_names = node_to_rel_map[node];
			for (int rel_index = 0; rel_index < rel_names.size(); ++rel_index) 
				mp_is_node_integrated_before[mp_Rel_to_Node[rel_names[rel_index]]] = true;
			Logical_TreeNode* Join_node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_JOIN);
			Join_node->u.JOIN.left = Final;  //����join����滻ԭ��������Ҷ�ڵ�
			Join_node->u.JOIN.right = node;
			Final = Join_node;
		}
		if(Attrs.size()!=0)
		Root->kind = Logical_TreeNode_Kind::PLAN_PROJ;  //��������ͶӰ
		Root->u.PROJECTION.rel = Final;
		Attr_Info* attrs = new Attr_Info[Attrs.size()];
		for (int i = 0; i < Attrs.size(); i++) {
			attrs[i] = Attrs[i];
		}
		Root->u.PROJECTION.Attr_Num = Attrs.size();
		Root->u.PROJECTION.Attr_list = attrs;
	}
	display();
	return Root;
}

Logical_TreeNode* Logical_Tree_Builder::get_tree_root_with_order(vector<string> order)
{
	if (order.empty()) return NULL;
	Logical_TreeNode* Root_in_func = relation_to_node_map[order[0]];
	
	for (int i = 1; i < order.size(); ++i) {
		Logical_TreeNode* join_node = get_logical_tree_node(PLAN_JOIN);
		join_node->u.JOIN.left = Root_in_func;
		join_node->u.JOIN.right = relation_to_node_map[order[i]];
		for (int j = 0; j < i; ++j) 
			join_node = add_node_to_binary_condition_node(join_node, order[i], order[j]);
		Root_in_func = join_node;
	}

	return Root_in_func;
}
Logical_TreeNode* Logical_Tree_Builder::add_node_to_binary_condition_node(Logical_TreeNode* node, string rel1, string rel2) {
	vector<string> rels = { rel1, rel2 };
	sort(rels.begin(), rels.end());
	if (!relation_to_binary_condition_node_map.count(rels)) return node;
	Logical_TreeNode* p = relation_to_binary_condition_node_map[rels], * p0 = NULL;
	Logical_TreeNode* new_binary_tree, * root(NULL), * rear(NULL);
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
void Logical_Tree_Builder::display()
{
	Logical_TreeNode* node = Root;
	queue<Logical_TreeNode*> que1;
	queue<Logical_TreeNode*> que2;
	que1.push(node);
	while (true) {
		while (!que1.empty()) {
			node = que1.front();
			que1.pop();
			switch (node->kind) {
			case Logical_TreeNode_Kind::PLAN_FILESCAN: {
				cout << "scan " << node->u.FILESCAN.Rel << "  ";
				break;
			}
			case Logical_TreeNode_Kind::PLAN_FILTER: {
				cout << "filter " << "  ";
				Condition* cond = (Condition*)node->u.FILTER.expr_filter;
				cout << cond->lhsAttr.attrname << " xxx ";
				if (cond->bRhsIsAttr) {
					cout << cond->rhsAttr.relname << "." << cond->rhsAttr.attrname << endl;
				}
				else {
					cout << *(int*)cond->rhsValue.data << endl;
				}
				que2.push(node->u.FILTER.rel);
				break;
			}
			case Logical_TreeNode_Kind::PLAN_JOIN: {
				cout << "join" << "  ";
				que2.push(node->u.JOIN.left);
				que2.push(node->u.JOIN.right);
				break;
			}
			case Logical_TreeNode_Kind::PLAN_PROJ: {
				cout << "proj" << "  ";
				que2.push(node->u.PROJECTION.rel);
				break;
			}
			}
		}
		if (que2.empty()) {
			break;
		}
		que1.swap(que2);
		cout << endl;
	}

}
void Logical_Tree_Builder::delete_node(Logical_TreeNode* root) {
	if (root->kind == PLAN_FILTER) delete_node(root->u.FILTER.rel);
	if (root->kind == PLAN_JOIN) {
		delete_node(root->u.JOIN.left);
		delete_node(root->u.JOIN.right);
	}
	if (root->kind == PLAN_PROJ) delete_node(root->u.PROJECTION.rel);
	//�ͷŸý��ռ�
	//delete root;
}

