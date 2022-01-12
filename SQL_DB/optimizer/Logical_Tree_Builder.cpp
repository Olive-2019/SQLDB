#include "Logical_Tree_Builder.h"
#include "../Subsystem1.h"
#include <iostream>
#include <map>
using namespace std;

Logical_Tree_Builder::Logical_Tree_Builder(vector<Rel_Info> Rels, vector<Attr_Info> Attrs, vector<Condition> Conds)
{
	/*
	警告

	若两个查询条件涉及到的数据表一致，该树会导致大量的空间浪费，
	因为会将两表连接后的新表 自身与自身进行连接
	
	尚未测试
	*/


	Root = new Logical_TreeNode();
	/*
	建立逻辑树
	*/

	
	
	map<string, Logical_TreeNode*> mp_Rel_to_Node;  //根据表名查找结点
	
	for (int i = 0; i < Rels.size(); i++) {   //建立叶节点
		Logical_TreeNode* node = new Logical_TreeNode();
		node->kind = Logical_TreeNode_Kind::PLAN_FILESCAN;
		node->u.FILESCAN.Rel = Rels[i];
		mp_Rel_to_Node[Rels[i].Rel_Name] = node;
	}

	
	for (int i = 0; i < Conds.size(); i++) {  //根据所有一元条件建立初步的filter
		if (Conds[i].bRhsIsAttr == false) {
			Rel_Info temp;
			Subsystem1_Manager::BASE.lookup_Rel(Conds[i].lhsAttr.relname, temp);
			Logical_TreeNode* leaf = mp_Rel_to_Node[temp.Rel_Name];
			Logical_TreeNode* node = new Logical_TreeNode();
			node->kind = Logical_TreeNode_Kind::PLAN_FILTER;
			node->u.FILTER.rel = leaf;
			node->u.FILTER.expr_filter = Conds[i];  //利用filter结点替换原本的叶节点
			mp_Rel_to_Node[temp.Rel_Name] = node;
		}
	}
	
	for (int i = 0; i < Conds.size(); i++) {  //根据二元条件进一步建立逻辑树
		if (Conds[i].bRhsIsAttr == true) {
			Rel_Info Ltemp,Rtemp;
			Subsystem1_Manager::BASE.lookup_Rel(Conds[i].lhsAttr.relname, Ltemp);
			Subsystem1_Manager::BASE.lookup_Rel(Conds[i].rhsAttr.relname, Rtemp);
			Logical_TreeNode* Lnode = mp_Rel_to_Node[Ltemp.Rel_Name];
			Logical_TreeNode* Rnode = mp_Rel_to_Node[Rtemp.Rel_Name];
			Logical_TreeNode* node = new Logical_TreeNode();
			node->kind = Logical_TreeNode_Kind::PLAN_JOIN;
			node->u.JOIN.left = Lnode;  //利用join结点替换原本的两个叶节点
			node->u.JOIN.right = Rnode;

			Logical_TreeNode* Filter_Node = new Logical_TreeNode(); //两表连接后加一层filter条件
			Filter_Node->kind = Logical_TreeNode_Kind::PLAN_FILTER;
			Filter_Node->u.FILTER.rel = node;
			Filter_Node->u.FILTER.expr_filter = Conds[i];

			mp_Rel_to_Node[Ltemp.Rel_Name] = Filter_Node;
			mp_Rel_to_Node[Rtemp.Rel_Name] = Filter_Node;

		}
	}

	
	{
		//进行最后的整理，将所有表连接起来
		Rel_Info temp = Rels[0];
		Logical_TreeNode* Final = mp_Rel_to_Node[temp.Rel_Name];
		map<Logical_TreeNode*, bool> mp_is_node_integrated_before;//该结点是否已被整合
		mp_is_node_integrated_before[Final] = true;
		for (int i = 1; i < Rels.size(); i++) {
			Logical_TreeNode* node = mp_Rel_to_Node[Rels[i].Rel_Name];
			if (mp_is_node_integrated_before.count(node)) {
				continue;
			}
			mp_is_node_integrated_before[node] = true;
			Logical_TreeNode* Join_node = new Logical_TreeNode();
			Join_node->kind = Logical_TreeNode_Kind::PLAN_JOIN;
			Join_node->u.JOIN.left = Final;  //利用join结点替换原本的两个叶节点
			Join_node->u.JOIN.right = node;
			Final = Join_node;
		}

		Root->kind = Logical_TreeNode_Kind::PLAN_PROJ;  //进行最后的投影
		Root->u.PROJECTION.rel = Final;
		Root->u.PROJECTION.Attr_list = Attrs;
	}
}
