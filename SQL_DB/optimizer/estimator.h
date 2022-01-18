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
	vector<Condition> Conds;//一元条件
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
	//记录数量估计
	int estimate_record_num(const Condition& cond, int record_num);
	Logical_TreeNode* Root;
	Link_Lost estimate_TreeNode_Lost_Scan(Logical_TreeNode* node);
	Link_Lost estimate_TreeNode_Lost_Filter(Logical_TreeNode* node);
	Link_Lost estimate_TreeNode_Lost_Join(Logical_TreeNode* node);
	Link_Lost estimate_TreeNode_Lost_Proj(Logical_TreeNode* node);

	Link_Lost estimate_TreeNode_Lost(Logical_TreeNode* node);

	Link_Lost estimate_scan_lost(const Execution_Plan& Plan);

public:
	Estimator(Logical_TreeNode* Root) {
		this->Root = Root;
	}

	double estimate();

};