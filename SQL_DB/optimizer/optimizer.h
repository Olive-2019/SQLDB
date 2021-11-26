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
	vector<Execution_Plan> get_Link_Order();

};