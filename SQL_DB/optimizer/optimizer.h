#pragma once
#include "../global.h"
#include "../parser/parser_interp.h"
#include "estimator.h"
#include "Logical_Tree_Builder.h"
#include "../executor/executor.h"
class Optimizer {

private:

	Logical_TreeNode* Logical_Tree_Root;  //逻辑查询树（关系代数）

	Executor* executor;

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
	//是否二元条件
	bool IsBinary(Condition cond); 

	void init();

public:
	//select语句的构造函数
	Optimizer(int Rel_num, RelInfo* rels, int Attr_num, AggRelAttr* attrs,
		int Cond_num, Condition* conds, SQL_type sql_type = SQL_type::SELECT);
	//update语句构造函数,char**values为新值
	Optimizer(RelInfo rel, int Attr_num, AggRelAttr* attrs, char**values,
		int Cond_num, Condition* conds, SQL_type sql_type = SQL_type::UPDATE);
	//insert语句构造函数
	Optimizer(RelInfo rel, int Attr_num, AggRelAttr* attrs, char** values,
		SQL_type sql_type = SQL_type::INSERT);
	//delete语句构造函数
	Optimizer(RelInfo rel, int Cond_num, Condition* conds,
		SQL_type sql_type = SQL_type::DELETE);

	vector<Execution_Plan> get_Link_Order();

};