#pragma once
#include "../global.h"
#include "../parser/parser_interp.h"

enum select_type {
	SIMPLE, UNION, PRIMARY, SUBQUERY, DERIVED, UNION_RESULT
};

struct Execution_Plan {
	Rel_Info Rel;
	vector<Index_Info> Index;
	vector<Condition> Conds;
	select_type type;
};

class Optimizer {

private:
	vector<Rel_Info> Rels;
	vector<Attr_Info> Attrs;
	vector<Condition> Conds;
	vector<Index_Info> Indexes;
	vector<Condition> Binary_Cond;

	vector<Execution_Plan> Plans;
	vector<int> Link_Order;

	void lookup_Indexes();
	void generate_execution_plan();
	void generate_link_order();
	int estimate_order_lost(vector<int> Order);
	int estimate_link_lost(Execution_Plan Plan1, Execution_Plan Plan2, vector<Condition> Cond);
	int Scan_Lost(Execution_Plan Plan);

public:
	vector<Execution_Plan> get_Link_Order();

};