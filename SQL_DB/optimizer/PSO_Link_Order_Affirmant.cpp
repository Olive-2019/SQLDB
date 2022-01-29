#include "PSO_Link_Order_Affirmant.h"

vector<Condition> PSO_Link_Order_Affirmant::PSO()
{

	return vector<Condition>();
}

PSO_Link_Order_Affirmant::PSO_Link_Order_Affirmant(vector<Condition>& Conds, int max_iter)
	:Link_Order_Affirmant(Conds), max_iter(max_iter) { }

vector<Condition> PSO_Link_Order_Affirmant::get_Link_Order()
{
	return PSO();
}
