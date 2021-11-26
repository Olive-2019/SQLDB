#ifndef PARSER_INTERP_H
#define PARSER_INTERP_H
#include <iostream>
#include "parser.h"
#include "parser_node.h"
using namespace std;

struct AttrInfo {
	char *attrname;
	AttrType type;
	int len;
};

struct Value {
	AttrType type;
	void* data;
	friend ostream& operator<<(ostream& os, const Value& v);
};

struct AggRelAttr {
	AggFun func;
	char *relname;  /* 表名 */
	char *attrname; /* 属性名 */
	friend std::ostream &operator<<(std::ostream &s, const AggRelAttr &ra);
};

struct RelAttr {
	string relname;
	string attrname;
	friend std::ostream &operator<<(std::ostream &s, const RelAttr &ra);
};

struct Condition {
	RelAttr  lhsAttr;    //左边的属性
	int op;              //运算符
	bool      bRhsIsAttr;//标志右边是值还是属性，true表示属性
	RelAttr  rhsAttr;    //右边的属性
	Value    rhsValue;   //右边的值

	friend std::ostream &operator<<(std::ostream &s, const Condition &c);

};

int interp(NODE *n);
#endif /* PARSER_INTERP_H */

