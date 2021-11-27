#pragma once
#include "../global.h"
#include "../parser/parser_interp.h"

enum select_type {
	SIMPLE, UNION, PRIMARY, SUBQUERY, DERIVED, UNION_RESULT
};

struct Execution_Plan {
	Rel_Info Rel;
	vector<Index_Info> Index;
	vector<Condition> Conds;//һԪ����
	select_type type;
};

class Optimizer {

private:
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
	int estimate_order_lost(vector<int> Order);
	int estimate_link_lost(Execution_Plan Plan1, Execution_Plan Plan2, vector<Condition> Cond);
	int Scan_Lost(Execution_Plan Plan);

	//�Ƿ��Ԫ����
	bool IsBinary(Condition cond); 

public:
	Optimizer(const vector<Rel_Info>& Rels, const vector<Attr_Info>& Attrs, const vector<Condition>& Conds) {
		//����һԪ�Ͷ�Ԫ����
		vector<Condition> cond_temp;
		for (int cond_cnt = 0; cond_cnt < Conds.size(); ++cond_cnt) 
			if (Conds[cond_cnt].bRhsIsAttr) Binary_Cond.push_back(Conds[cond_cnt]);
			else cond_temp.push_back(Conds[cond_cnt]);
		//װ��ִ�мƻ�
		for (int plan_cnt = 0; plan_cnt < Rels.size(); ++plan_cnt) {
			Execution_Plan plan;
			plan.Rel = Rels[plan_cnt];
			plan.type = SIMPLE;//�Ȳ�������ڲ����
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