#include "Link_Order_Affirmant.h"
#include "Logical_Tree_Builder.h"
#include "optimizer.h"
#include "estimator.h"
#include <map>



Link_Order_Affirmant::Link_Order_Affirmant(vector<Rel_Info>& Rels, vector<Condition>& Conds)
	: Rels(Rels), Conds(Conds) {init();}

void Link_Order_Affirmant::init() {
	for (int i = 0; i < Rels.size(); i++) {
		Logical_TreeNode* node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILESCAN);
		node->u.FILESCAN.Rel = Rels[i].Rel_Name;
		relation_to_node_map[Rels[i].Rel_Name] = node;
	}

	for (int cond_index = 0; cond_index < Conds.size(); ++cond_index) {
		//cout << relation_to_binary_condition_node_map.size() << endl;
		//根据所有一元条件建立初步的filter
		if (Conds[cond_index].bRhsIsAttr == false) {
			Logical_TreeNode* node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILTER);
			node->u.FILTER.rel = relation_to_node_map[Conds[cond_index].lhsAttr.relname];
			node->u.FILTER.expr_filter = new Condition;
			memcpy(node->u.FILTER.expr_filter, &(Conds[cond_index]), sizeof(Condition));//利用filter结点替换原本的叶节点
			relation_to_node_map[Conds[cond_index].lhsAttr.relname] = node;
		}
		//二元条件的整理
		else {
			vector<string> rels = { Conds[cond_index].lhsAttr.relname, Conds[cond_index].rhsAttr.relname };
			//左右都是同一张表的
			if (rels[0] == rels[1]) {
				Logical_TreeNode* Filter_Node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILTER); //filter条件
				Filter_Node->u.FILTER.rel = relation_to_node_map[rels[0]];
				Filter_Node->u.FILTER.expr_filter = new Condition;
				memcpy(Filter_Node->u.FILTER.expr_filter, &Conds[cond_index], sizeof(Condition));
				relation_to_node_map[rels[0]] = Filter_Node;
			}
			else {
				sort(rels.begin(), rels.end());
				Logical_TreeNode* Filter_Node = get_logical_tree_node(Logical_TreeNode_Kind::PLAN_FILTER);
				Filter_Node->u.FILTER.expr_filter = new Condition;
				memcpy((char*)Filter_Node->u.FILTER.expr_filter, (char*)&(Conds[cond_index]), sizeof(Conds[cond_index]));

				if (!relation_to_binary_condition_node_map.count(rels)) {
					Filter_Node->u.FILTER.rel = NULL;
					relation_to_binary_condition_node_map[rels] = Filter_Node;
					//relation_to_binary_condition_node_map.insert(map<vector<string>, Logical_TreeNode*>::value_type(rels, Filter_Node));
				}
				else {
					Filter_Node->u.FILTER.rel = relation_to_binary_condition_node_map[rels];
					relation_to_binary_condition_node_map[rels] = Filter_Node;
				}

			}
		}
	}

	for (int i = 0; i < Rels.size(); ++i) rels_name.push_back(Rels[i].Rel_Name);

	
}



//Link_Order_Affirmant::Link_Order_Affirmant(vector<Condition>& Conds)
//{
//	for (int i = 0; i < Conds.size(); i++) {
//		if (Conds[i].bRhsIsAttr) {
//			this->Conds_2.push_back(Conds[i]);
//		}
//		else {
//			this->Conds_1.push_back(Conds[i]);
//		}
//	}
//}
//
//vector<Condition> Link_Order_Affirmant::get_Link_Order()
//{
//	int Cond_Num = Conds_2.size();
//	vector<Condition> ret;
//	double Min_Lost = 10000000;
//	for (int time = 0; time < 100; time++) {
//		vector<int> temp;
//		map<int, bool> mp;
//		for (int i = 0; i < Cond_Num; i++) {
//			int t;  //随机初始化一个链接顺序
//			while (true) {
//				t = rand() % Cond_Num;
//				if (!mp.count(t)) {
//					mp[t] = true;
//					break;
//				}
//			}
//			temp.push_back(t);
//		}
//		vector<Condition> Cond_Temp;
//		for (int i = 0; i < Cond_Num; i++) {
//			Cond_Temp.push_back(Conds_2[temp[i]]);
//		}
//		for (int i = 0; i < Conds_1.size(); i++) {
//			Cond_Temp.push_back(Conds_1[i]);
//		}
//		Logical_Tree_Builder* tree_builder = new Logical_Tree_Builder(Rels, Attrs, Cond_Temp);
//		Estimator* estimator = new Estimator(tree_builder->get_tree_root());
//		double lost = estimator->estimate();
//		if (lost < Min_Lost) {
//			Min_Lost = lost;
//			ret = Cond_Temp;
//		}
//	}
//	return ret;
//}