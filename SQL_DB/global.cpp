#include "global.h"
string Global_Paras:: Current_DB;
string Global_Paras:: Current_User;
int Global_Paras:: Block_Size = 10;
#include "parser/parser_token.h"

double EVENLY::rate(int op, double value) {
	switch (op)
	{
	case TOKENKIND::T_EQ: 
		if (value <= MAX && value >= MIN) return 1 / (MAX - MIN) * (0.05);
		return 0;
	case TOKENKIND::T_NE: 
		return 1 - rate(TOKENKIND::T_EQ, value);
	case TOKENKIND::T_GE:
	case TOKENKIND::T_GT: 
		if (value < MAX) return 1 / (MAX - MIN) * (MAX - max(MIN, value));
		return 0;
	default:
		return 1 - rate(TOKENKIND::T_GE, value);
	}
}


