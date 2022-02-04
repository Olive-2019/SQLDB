#pragma once
#include "Link_Order_Affirmant.h"
#include "../parser/parser_interp.h"
//class PSO_Link_Order_Affirmant 
//	:public Link_Order_Affirmant 
//{
//private:
//	vector<Condition> PSO();
//
//	vector<Condition*> Conds;
//	int num_of_particle;
//	const int max_iter;
//
//	vector<int> gbest;
//	vector<vector<int>> pbest;
//	vector<vector<double>> V;
//	void init();
//	void update();
//	int estimate(vector<int>& order);
//
//public:
//	PSO_Link_Order_Affirmant(vector<Condition>& Conds, int max_iter = 20);
//	virtual vector<Condition> get_Link_Order();
//};