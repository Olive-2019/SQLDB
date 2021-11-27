#pragma once
#include "../global.h"
#include "../parser/parser_interp.h"

enum select_type {
	SIMPLE, UNION, PRIMARY, SUBQUERY, DERIVED, UNION_RESULT
};

struct Execution_Plan {
	Rel_Info Rel;
	vector<Index_Info> Index;
	vector<Condition> Conds;//一元条件
	select_type type;
};

class Optimizer {

private:
	//未用到的属性
	vector<Rel_Info> Rels;
	vector<Attr_Info> Attrs;
	vector<Condition> Conds;
	vector<Index_Info> Indexes;
	//未用到的属性

	vector<Condition> Binary_Cond;//二元条件

	vector<Execution_Plan> Plans;//执行计划
	vector<int> Link_Order;//顺序
	//查找索引，若存在返回true，并将index设为找到的索引
	bool lookup_Index(string RelName, string AttrName, Index_Info& index);
	void generate_execution_plan();
	void generate_link_order();
	int estimate_order_lost(vector<int> Order);
	int estimate_link_lost(Execution_Plan Plan1, Execution_Plan Plan2, vector<Condition> Cond);
	int Scan_Lost(Execution_Plan Plan);

	//是否二元条件
	bool IsBinary(Condition cond); 

public:
	Optimizer(const vector<Rel_Info>& Rels, const vector<Attr_Info>& Attrs, const vector<Condition>& Conds) {
		//区分一元和二元条件
		vector<Condition> cond_temp;
		for (int cond_cnt = 0; cond_cnt < Conds.size(); ++cond_cnt) 
			if (Conds[cond_cnt].bRhsIsAttr) Binary_Cond.push_back(Conds[cond_cnt]);
			else cond_temp.push_back(Conds[cond_cnt]);
		//装填执行计划
		for (int plan_cnt = 0; plan_cnt < Rels.size(); ++plan_cnt) {
			Execution_Plan plan;
			plan.Rel = Rels[plan_cnt];
			plan.type = SIMPLE;//先不管外层内层的事
			vector<Condition>::iterator it = cond_temp.begin();
			while (it != cond_temp.end()) {
				if (!strcmp(plan.Rel.Rel_Name, it->lhsAttr.relname)) {
					plan.Conds.push_back(*it);
					vector<Condition>::iterator it_temp = it;
					it++;
					cond_temp.erase(it_temp);
					Index_Info index_info;
					if (lookup_Index(plan.Rel.Rel_Name, it->lhsAttr.attrname, index_info)) 
						plan.Index.push_back(index_info);
				}
				else it++;
			}

		}
		
	}
	vector<Execution_Plan> get_Link_Order();

};