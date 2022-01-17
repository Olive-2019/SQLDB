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
struct RelInfo {
	char* relname;
	char* nickname;
};
struct Value {
	AttrType type;
	void* data;
	friend ostream& operator<<(ostream& os, const Value& v);
};



struct RelAttr {
	char* relname;
	char* attrname;
	friend std::ostream& operator<<(std::ostream& s, const RelAttr& ra);
};

struct Condition {
	RelAttr  lhsAttr;    //��ߵ�����
	int op;              //�����
	bool      bRhsIsAttr;//��־�ұ���ֵ�������ԣ�true��ʾ����
	RelAttr  rhsAttr;    //�ұߵ�����
	Value    rhsValue;   //�ұߵ�ֵ

	friend std::ostream &operator<<(std::ostream &s, const Condition &c);
};
struct AggRelAttr {
	AggFun func;
	char* relname;  /* ���� */
	char* attrname; /* ������ */
	friend std::ostream& operator<<(std::ostream& s, const AggRelAttr& ra);
};
int interp(NODE *n);
#endif /* PARSER_INTERP_H */

