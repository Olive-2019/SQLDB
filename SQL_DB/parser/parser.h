#ifndef PARSER_H
#define PARSER_H
#include "redbase.h"

#define PROMPT	"\nSQL >> "

RC RBparse();
void parse(string sql);
#endif 