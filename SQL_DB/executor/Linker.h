#pragma once
#include <iostream>
#include "../optimizer/estimator.h"
#include "../Subsystem1.h"
using namespace std;
class Linker {
private:
	Reader* Table1_Reader;
	Reader* Table2_Reader;
public:
	Linker(Execution_Plan& plan1, Execution_Plan plan2, vector<Condition> Conds);
	void Link();
	vector<RID> get_RID_by_Cond(Execution_Plan& plan, vector<Condition>& cond);
	void sortRID();//����a.x<b.y��ɨ��a�����򣬱�����ɨ��a
	string Save_as_Table();
};