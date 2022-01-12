#pragma once
#include "optimizer.h"
#include "estimator.h"
class Link_Order_Affirmant {
protected:

	const vector<Execution_Plan>& Plans;
	Estimator estimator;

public:

	Link_Order_Affirmant(vector<Execution_Plan>& Plans);

	virtual vector<Execution_Plan> get_Link_Order();

};