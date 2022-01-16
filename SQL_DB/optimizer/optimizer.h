#pragma once
#include "../global.h"
#include "../parser/parser_interp.h"
#include "estimator.h"
#include "Logical_Tree_Builder.h"
#include "../executor/executor.h"
class Optimizer {

private:

	Logical_TreeNode* Logical_Tree_Root;  //�߼���ѯ������ϵ������

	Executor* executor;

	//δ�õ�������
	vector<Rel_Info> Rels;
	vector<Attr_Info> Attrs;
	vector<Condition> Conds;
	vector<Index_Info> Indexes;
	//δ�õ�������

	vector<Condition> Binary_Cond;//��Ԫ����

	vector<Execution_Plan> Plans;//ִ�мƻ�
	vector<int> Link_Order;//˳��





	//���������������ڷ���true������index��Ϊ�ҵ�������
	bool lookup_Index(string RelName, string AttrName, Index_Info& index);
	void generate_execution_plan();
	void generate_link_order();
	//�Ƿ��Ԫ����
	bool IsBinary(Condition cond); 

	void init();

public:
	//select���Ĺ��캯��
	Optimizer(int Rel_num, RelInfo* rels, int Attr_num, AggRelAttr* attrs,
		int Cond_num, Condition* conds, SQL_type sql_type = SQL_type::SELECT);
	//update��乹�캯��,char**valuesΪ��ֵ
	Optimizer(RelInfo rel, int Attr_num, AggRelAttr* attrs, char**values,
		int Cond_num, Condition* conds, SQL_type sql_type = SQL_type::UPDATE);
	//insert��乹�캯��
	Optimizer(RelInfo rel, int Attr_num, AggRelAttr* attrs, char** values,
		SQL_type sql_type = SQL_type::INSERT);
	//delete��乹�캯��
	Optimizer(RelInfo rel, int Cond_num, Condition* conds,
		SQL_type sql_type = SQL_type::DELETE);

	vector<Execution_Plan> get_Link_Order();

};