#pragma once
#include "Link_Order_Affirmant.h"
class PSO_Link_Order_Affirmant 
	:public Link_Order_Affirmant 
{
private:
	vector<Execution_Plan> PSO();
	int num_of_particle;
	int num_of_Rel;
	vector<int> gbest;
	vector<vector<int>> pbest;
	vector<vector<double>> V;
	void init();
	void update();
	int estimate(vector<int>& order);

public:
	vector<Execution_Plan> get_Link_Order();
};