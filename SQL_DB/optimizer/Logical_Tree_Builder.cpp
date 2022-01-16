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
	//警告基本解决
	/*
	警告

	若两个查询条件涉及到的数据表一致，该树会导致大量的空间浪费，
	因为会将两表连接后的新表 自身与自身进行连接

	尚未测试
	*/
	//初步测试

	Root = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_PROJ);
	/*
	建立逻辑树
	*/

	map<string, Logical_TreeNode*> mp_Rel_to_Node;  //根据表名查找结点
	//双向快表，解决警告问题（这个之前我查过，属于数据库常用技术了【空间换时间】）
	map<Logical_TreeNode*, vector<string>> node_to_rel_map;//根据结点找表名
	for (int i = 0; i < Rels.size(); i++) {   //建立叶节点
		cout << Rels[i].Rel_Name << endl;
		Logical_TreeNode* node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILESCAN);
		node->u.FILESCAN.Rel = Rels[i].Rel_Name;
		mp_Rel_to_Node[Rels[i].Rel_Name] = node;
		vector<string> rel_names = { Rels[i].Rel_Name };
		node_to_rel_map[node] = rel_names;
	}
	for (int i = 0; i < Conds.size(); i++) {  //根据所有一元条件建立初步的filter
		if (Conds[i].bRhsIsAttr == false) {
			Rel_Info temp;
			Subsystem1_Manager::BASE.lookup_Rel(Conds[i].lhsAttr.relname, temp);
			cout << "cond RelName==" << temp.Rel_Name << endl;
			Logical_TreeNode* leaf = mp_Rel_to_Node[temp.Rel_Name];
			Logical_TreeNode* node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILTER);
			node->u.FILTER.rel = leaf;
			node->u.FILTER.expr_filter = new Condition;
			memcpy(node->u.FILTER.expr_filter, &(Conds[i]), sizeof(Condition));//利用filter结点替换原本的叶节点
			node_to_rel_map[node] = node_to_rel_map[leaf];
			node_to_rel_map.erase(node_to_rel_map.find(leaf));
			mp_Rel_to_Node[Conds[i].lhsAttr.relname] = node;
		}
	}

	//修改：两个涉及表相同的二元条件出现时，不用再join
	
	for (int i = 0; i < Conds.size(); i++) {  //根据二元条件进一步建立逻辑树
		if (Conds[i].bRhsIsAttr == true) {
			Rel_Info Ltemp, Rtemp;
			Subsystem1_Manager::BASE.lookup_Rel(Conds[i].lhsAttr.relname, Ltemp);
			Subsystem1_Manager::BASE.lookup_Rel(Conds[i].rhsAttr.relname, Rtemp);
			Logical_TreeNode* Lnode = mp_Rel_to_Node[Ltemp.Rel_Name];
			Logical_TreeNode* Rnode = mp_Rel_to_Node[Rtemp.Rel_Name];
			
			if (Lnode != Rnode) {
				Logical_TreeNode* node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_JOIN);
				node->u.JOIN.left = Lnode;  //利用join结点替换原本的两个叶节点
				node->u.JOIN.right = Rnode;

				Logical_TreeNode* Filter_Node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILTER); //两表连接后加一层filter条件
				Filter_Node->u.FILTER.rel = node;
				
				Filter_Node->u.FILTER.expr_filter = new Condition;
				memcpy(Filter_Node->u.FILTER.expr_filter, &Conds[i], sizeof(Condition));
				//两结点对应关系的合并
				vector<string> LRel = node_to_rel_map[Lnode], RRel = node_to_rel_map[Rnode];
				LRel.insert(LRel.end(), RRel.begin(), RRel.end());
				unique(LRel.begin(), LRel.end());
				node_to_rel_map[Filter_Node] = LRel;
				//删掉原来的
				node_to_rel_map.erase(node_to_rel_map.find(Lnode));
				node_to_rel_map.erase(node_to_rel_map.find(Rnode));
				//更新对应的结点
				for (int Rel_index = 0; Rel_index < LRel.size(); ++Rel_index) 
					mp_Rel_to_Node[LRel[Rel_index]] = Filter_Node;
			}
			else {
				Logical_TreeNode* Filter_Node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILTER); //两表连接后加一层filter条件
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
			vector<string> rel_names = node_to_rel_map[node];
			for (int rel_index = 0; rel_index < rel_names.size(); ++rel_index) 
				mp_is_node_integrated_before[mp_Rel_to_Node[rel_names[rel_index]]] = true;
			Logical_TreeNode* Join_node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_JOIN);
			Join_node->u.JOIN.left = Final;  //利用join结点替换原本的两个叶节点
			Join_node->u.JOIN.right = node;
			Final = Join_node;
		}
		if(Attrs.size()!=0)
		Root->kind = Logical_TreeNode_Kind::PLAN_PROJ;  //进行最后的投影
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

Logical_TreeNode* Logical_Tree_Builder::get_tree_root_order(vector<string> order)
{

	//Logical_TreeNode* Root;
	/*
	警告

	若两个查询条件涉及到的数据表一致，该树会导致大量的空间浪费，
	因为会将两表连接后的新表 自身与自身进行连接

	尚未测试
	*/


	Root = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_PROJ);
	/*
	建立逻辑树
	*/

	map<string, Logical_TreeNode*> mp_Rel_to_Node;  //根据表名查找结点

	for (int i = 0; i < Rels.size(); i++) {   //建立叶节点
		cout << Rels[i].Rel_Name << endl;
		Logical_TreeNode* node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILESCAN);
		node->u.FILESCAN.Rel = Rels[i].Rel_Name;
		mp_Rel_to_Node[Rels[i].Rel_Name] = node;
	}


	for (int i = 0; i < Conds.size(); i++) {  //根据所有一元条件建立初步的filter
		if (Conds[i].bRhsIsAttr == false) {
			Rel_Info temp;
			Subsystem1_Manager::BASE.lookup_Rel(Conds[i].lhsAttr.relname, temp);
			cout << "cond RelName==" << temp.Rel_Name << endl;
			Logical_TreeNode* leaf = mp_Rel_to_Node[temp.Rel_Name];
			Logical_TreeNode* node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILTER);
			node->u.FILTER.rel = leaf;
			node->u.FILTER.expr_filter = new Condition;
			memcpy(node->u.FILTER.expr_filter, &(Conds[i]), sizeof(Condition));//利用filter结点替换原本的叶节点
			mp_Rel_to_Node[Conds[i].lhsAttr.relname] = node;
		}
	}


	Logical_TreeNode* Final = mp_Rel_to_Node[order[0]];
	map<string, bool> mp_is_node_integrated_before;
	mp_is_node_integrated_before[order[0]] = true;
	for (int orderi = 1; orderi < order.size(); orderi++) {
		Logical_TreeNode* node= mp_Rel_to_Node[order[orderi]];
		Logical_TreeNode* join_node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_JOIN);
		join_node->u.JOIN.left = Final;
		join_node->u.JOIN.right = node;
		Final = join_node;
		for (int i = 0; i < Conds.size(); i++) {
			if (!Conds[i].bRhsIsAttr) continue;
			if ((mp_is_node_integrated_before.count(Conds[i].lhsAttr.relname)
				&& order[i] == Conds[i].rhsAttr.relname)
				|| (mp_is_node_integrated_before.count(Conds[i].rhsAttr.relname)
					&& order[i] == Conds[i].lhsAttr.relname))
			{
				Logical_TreeNode* filter_node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILTER);
				filter_node->u.FILTER.rel = Final;
				filter_node->u.FILTER.expr_filter = new Condition;
				memcpy(filter_node->u.FILTER.expr_filter, &Conds[i], sizeof(Condition));

				Final = filter_node;
			}
		}
		mp_is_node_integrated_before[order[orderi]] = true;
	}
	Root->kind = Logical_TreeNode_Kind::PLAN_PROJ;  //进行最后的投影
	Root->u.PROJECTION.rel = Final;
	Attr_Info* attrs = new Attr_Info[Attrs.size()];
	for (int i = 0; i < Attrs.size(); i++) {
		attrs[i] = Attrs[i];
	}
	Root->u.PROJECTION.Attr_Num = Attrs.size();
	Root->u.PROJECTION.Attr_list = attrs;
	display();
	return Root;
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
