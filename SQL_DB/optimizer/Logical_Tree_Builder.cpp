#include "Logical_Tree_Builder.h"
#include "../Subsystem1.h"
#include <iostream>
#include <map>
using namespace std;

Logical_Tree_Builder::Logical_Tree_Builder(vector<Rel_Info> Rels, vector<Attr_Info> Attrs, vector<Condition> Conds)
{
	/*
	����

	��������ѯ�����漰�������ݱ�һ�£������ᵼ�´����Ŀռ��˷ѣ�
	��Ϊ�Ὣ�������Ӻ���±� �����������������
	
	��δ����
	*/


	Root = new Logical_TreeNode();
	/*
	�����߼���
	*/

	
	
	map<string, Logical_TreeNode*> mp_Rel_to_Node;  //���ݱ������ҽ��
	
	for (int i = 0; i < Rels.size(); i++) {   //����Ҷ�ڵ�
		Logical_TreeNode* node = new Logical_TreeNode();
		node->kind = Logical_TreeNode_Kind::PLAN_FILESCAN;
		node->u.FILESCAN.Rel = Rels[i];
		mp_Rel_to_Node[Rels[i].Rel_Name] = node;
	}

	
	for (int i = 0; i < Conds.size(); i++) {  //��������һԪ��������������filter
		if (Conds[i].bRhsIsAttr == false) {
			Rel_Info temp;
			Subsystem1_Manager::BASE.lookup_Rel(Conds[i].lhsAttr.relname, temp);
			Logical_TreeNode* leaf = mp_Rel_to_Node[temp.Rel_Name];
			Logical_TreeNode* node = new Logical_TreeNode();
			node->kind = Logical_TreeNode_Kind::PLAN_FILTER;
			node->u.FILTER.rel = leaf;
			node->u.FILTER.expr_filter = Conds[i];  //����filter����滻ԭ����Ҷ�ڵ�
			mp_Rel_to_Node[temp.Rel_Name] = node;
		}
	}
	
	for (int i = 0; i < Conds.size(); i++) {  //���ݶ�Ԫ������һ�������߼���
		if (Conds[i].bRhsIsAttr == true) {
			Rel_Info Ltemp,Rtemp;
			Subsystem1_Manager::BASE.lookup_Rel(Conds[i].lhsAttr.relname, Ltemp);
			Subsystem1_Manager::BASE.lookup_Rel(Conds[i].rhsAttr.relname, Rtemp);
			Logical_TreeNode* Lnode = mp_Rel_to_Node[Ltemp.Rel_Name];
			Logical_TreeNode* Rnode = mp_Rel_to_Node[Rtemp.Rel_Name];
			Logical_TreeNode* node = new Logical_TreeNode();
			node->kind = Logical_TreeNode_Kind::PLAN_JOIN;
			node->u.JOIN.left = Lnode;  //����join����滻ԭ��������Ҷ�ڵ�
			node->u.JOIN.right = Rnode;

			Logical_TreeNode* Filter_Node = new Logical_TreeNode(); //�������Ӻ��һ��filter����
			Filter_Node->kind = Logical_TreeNode_Kind::PLAN_FILTER;
			Filter_Node->u.FILTER.rel = node;
			Filter_Node->u.FILTER.expr_filter = Conds[i];

			mp_Rel_to_Node[Ltemp.Rel_Name] = Filter_Node;
			mp_Rel_to_Node[Rtemp.Rel_Name] = Filter_Node;

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
			mp_is_node_integrated_before[node] = true;
			Logical_TreeNode* Join_node = new Logical_TreeNode();
			Join_node->kind = Logical_TreeNode_Kind::PLAN_JOIN;
			Join_node->u.JOIN.left = Final;  //����join����滻ԭ��������Ҷ�ڵ�
			Join_node->u.JOIN.right = node;
			Final = Join_node;
		}

		Root->kind = Logical_TreeNode_Kind::PLAN_PROJ;  //��������ͶӰ
		Root->u.PROJECTION.rel = Final;
		Root->u.PROJECTION.Attr_list = Attrs;
	}
}
