#pragma once
#include "../global.h"
#include "../parser/parser_interp.h"
#include "Logical_Tree_Builder.h"
enum select_type {
	SIMPLE,
	UNION, 
	PRIMARY, 
	SUBQUERY, 
	DERIVED, 
	UNION_RESULT
};

struct Execution_Plan {
	Rel_Info Rel;
	vector<Index_Info> Index;
	vector<Condition> Conds;//һԪ����
	select_type type;
};

struct Link_Lost {
	int Disk_Scan_Lost;
	int Memory_Scan_Lost;
	int record_num;
	Link_Lost() {
		Disk_Scan_Lost = 0;
		Memory_Scan_Lost = 0;
		record_num = 0;
	}
	void merge(Link_Lost& LL) {
		this->Disk_Scan_Lost += LL.Disk_Scan_Lost;
		this->Memory_Scan_Lost += LL.Memory_Scan_Lost;
	}
};

class Estimator {

private:
	Logical_TreeNode* Root;

	Link_Lost estimate_TreeNode_Lost(Logical_TreeNode* node);

	Link_Lost estimate_link_lost_EQ(const Execution_Plan& Plan1,
		const Execution_Plan& Plan2, const vector<Condition>& Cond);

	Link_Lost estimate_link_lost_LT(const Execution_Plan& Plan1,
		const Execution_Plan& Plan2, const vector<Condition>& Cond);

	Link_Lost estimate_link_lost_GT(const Execution_Plan& Plan1,
		const Execution_Plan& Plan2, const vector<Condition>& Cond);

	Link_Lost estimate_order_lost(const vector<Execution_Plan>& Plan_Order);
	Link_Lost estimate_scan_lost(const Execution_Plan& Plan);
	int estimate_record_num(const Execution_Plan& Plan);
	Link_Lost estimate_link_lost(const Execution_Plan& Plan1, const Execution_Plan& Plan2, const vector<Condition>& Cond);

public:
	Estimator(Logical_TreeNode* Root) {
		this->Root = Root;
	}

	double estimate();

};