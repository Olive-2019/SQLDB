#pragma once
#include "optimizer.h"
#include "estimator.h"
#include "../parser/parser_interp.h"
class Link_Order_Affirmant {
protected:

	vector<Condition> Conds_2;

	vector<Condition> Conds_1;

	vector<Rel_Info> Rels;
	vector<Attr_Info> Attrs;
public:

	Link_Order_Affirmant(vector<Condition>& Conds);

	virtual vector<Condition> get_Link_Order();

};