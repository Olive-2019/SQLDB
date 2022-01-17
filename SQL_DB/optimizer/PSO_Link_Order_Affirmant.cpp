#include "PSO_Link_Order_Affirmant.h"

vector<Condition*> PSO_Link_Order_Affirmant::PSO()
{

	return;
}

PSO_Link_Order_Affirmant::PSO_Link_Order_Affirmant(vector<Condition*> Conds)
	:Link_Order_Affirmant(Conds)
{

}

vector<Condition*> PSO_Link_Order_Affirmant::get_Link_Order()
{
	return PSO();
}
