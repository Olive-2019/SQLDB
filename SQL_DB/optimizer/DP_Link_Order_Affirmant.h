#pragma once
#include "Link_Order_Affirmant.h"
class DP_Link_Order_Affirmant
	:public Link_Order_Affirmant
{
private:
	vector<Execution_Plan> DP();

public:
	vector<Execution_Plan> get_Link_Order();
};