#include "Link_Order_Affirmant.h"
#include "Logical_Tree_Builder.h"
#include "optimizer.h"
#include "estimator.h"
Link_Order_Affirmant::Link_Order_Affirmant(vector<Condition>& Conds)
{
	for (int i = 0; i < Conds.size(); i++) {
		if (Conds[i].bRhsIsAttr) {
			this->Conds_2.push_back(Conds[i]);
		}
		else {
			this->Conds_1.push_back(Conds[i]);
		}
	}
}

vector<Condition> Link_Order_Affirmant::get_Link_Order()
{
	int Cond_Num = Conds_2.size();
	vector<Condition> ret;
	double Min_Lost = 10000000;
	for (int time = 0; time < 100; time++) {
		vector<int> temp;
		map<int, bool> mp;
		for (int i = 0; i < Cond_Num; i++) {
			int t;  //随机初始化一个链接顺序
			while (true) {
				t = rand() % Cond_Num;
				if (!mp.count(t)) {
					mp[t] = true;
					break;
				}
			}
			temp.push_back(t);
		}
		vector<Condition> Cond_Temp;
		for (int i = 0; i < Cond_Num; i++) {
			Cond_Temp.push_back(Conds_2[temp[i]]);
		}
		for (int i = 0; i < Conds_1.size(); i++) {
			Cond_Temp.push_back(Conds_1[i]);
		}
		Logical_Tree_Builder* tree_builder = new Logical_Tree_Builder(Rels, Attrs, Cond_Temp);
		Estimator* estimator = new Estimator(tree_builder->get_tree_root());
		double lost = estimator->estimate();
		if (lost < Min_Lost) {
			Min_Lost = lost;
			ret = Cond_Temp;
		}
	}
	return ret;
}