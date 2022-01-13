#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "redbase.h"
#include "../global.h"
#include "parser_node.h"
#include "parser_interp.h"
#include "../optimizer/optimizer.h"
extern bool stop;

#define E_OK                0
#define E_INCOMPATIBLE      -1
#define E_TOOMANY           -2
#define E_NOLENGTH          -3
#define E_INVINTSIZE        -4
#define E_INVREALSIZE       -5
#define E_INVFORMATSTRING   -6
#define E_INVSTRLEN         -7
#define E_DUPLICATEATTR     -8
#define E_TOOLONG           -9
#define E_STRINGTOOLONG     -10

#define ERRFP stderr

/*
* print_error: prints an error message corresponding to errval
*/
static void print_error(const char *errmsg, RC errval)
{
	if (errmsg != NULL)
		fprintf(stderr, "%s: ", errmsg);
	switch (errval) {
	case E_OK:
		fprintf(ERRFP, "no error\n");
		break;
	case E_INCOMPATIBLE:
		fprintf(ERRFP, "attributes must be from selected relation(s)\n");
		break;
	case E_TOOMANY:
		fprintf(ERRFP, "too many elements\n");
		break;
	case E_NOLENGTH:
		fprintf(ERRFP, "length must be specified for STRING attribute\n");
		break;
	case E_INVINTSIZE:
		fprintf(ERRFP, "invalid size for INTEGER attribute (should be %d)\n",
			(int)sizeof(int));
		break;
	case E_INVREALSIZE:
		fprintf(ERRFP, "invalid size for REAL attribute (should be %d)\n",
			(int)sizeof(float));
		break;
	case E_INVFORMATSTRING:
		fprintf(ERRFP, "invalid format string\n");
		break;
	case E_INVSTRLEN:
		fprintf(ERRFP, "invalid length for string attribute\n");
		break;
	case E_DUPLICATEATTR:
		fprintf(ERRFP, "duplicated attribute name\n");
		break;
	case E_TOOLONG:
		fprintf(stderr, "relation name or attribute name too long\n");
		break;
	case E_STRINGTOOLONG:
		fprintf(stderr, "string attribute too long\n");
		break;
	default:
		fprintf(ERRFP, "unrecognized errval: %d\n", errval);
	}
}

/*
* parse_format_string: deciphers a format string of the form: xl
* where x is a type specification (one of `i' INTEGER, `r' REAL,
* `s' STRING, or `c' STRING (character)) and l is a length (l is
* optional for `i' and `r'), and stores the type in *type and the
* length in *len.
*
* Returns
*    E_OK on success
*    error code otherwise
*/
static int parse_format_string(char *format_string, AttrType *type, int *len)
{
	int n(1);
	char c = format_string[0];

	int len_format_string = strlen(format_string);
	char maybe_num = format_string[len_format_string - 1];
	if (maybe_num >= '0' && maybe_num <= '9') {
		int num_start = 0;
		char tmp = format_string[num_start];
		for (; tmp > '9' || tmp < '0'; num_start++) {
			tmp = format_string[num_start];
		}
		*len = atoi(format_string + num_start - 1);
		n = 2;
	}
	/* extract the components of the format string */
	//n = sscanf(format_string, "%c%d", &c, len);
	
	/* if no length given... */
	if (n == 1) {

		switch (c) {
		case 'i':		/* int */
			*type = INT;
			*len = sizeof(int);
			break;
		case 'f':
		case 'r':		/* float or real or double */
		case 'd':
			*type = FLOAT;
			*len = sizeof(float);
			break;
		case 's':		/* string */
			*type = STRING;
			break;
		case 'c':		/* char */
			return E_NOLENGTH;

		default:
			return E_INVFORMATSTRING;
		}
	}

	/* if both are given, make sure the length is valid */
	else if (n == 2) {

		switch (c) {
		case 'i':
			*type = INT;
			if (*len != sizeof(int))
				return E_INVINTSIZE;
			break;
		case 'f':
		case 'r':
			*type = FLOAT;
			if (*len != sizeof(float))
				return E_INVREALSIZE;
			break;
		case 's':
		case 'c':
			*type = STRING;
			
			if (*len < 1 || *len > MAXSTRINGLEN)
				return E_INVSTRLEN;
			
			break;
		default:
			return E_INVFORMATSTRING;
		}
	}

	/* otherwise it's not a valid format string */
	else
		return E_INVFORMATSTRING;

	return E_OK;
}

/*
* mk_attr_infos: converts a list of attribute descriptors (attribute names,
* types, and lengths) to an array of AttrInfo's so it can be sent to
* Create.
*
* Returns:
*    length of the list on success ( >= 0 )
*    error code otherwise
*/
static int mk_attr_infos(NODE *list, int max, vector<string>& column_name, vector<int>&types)
{
	int i;
	int len;
	AttrType type;
	NODE *n;
	RC errval;

	/* for each element of the list... */
	for (i = 0; list != NULL; ++i, list = list->u.LIST.next) {

		/* if the list is too long, then error */
		if (i == max)
			return E_TOOMANY;

		n = list->u.LIST.curr;

		/* Make sure the attribute name isn't too long */
		if (strlen(n->u.ATTRTYPE.attrname) > MAXNAME)
			return E_TOOLONG;

		/* interpret the format string */
		errval = parse_format_string(n->u.ATTRTYPE.type, &type, &len);
		if (errval != E_OK)
			return errval;

		/* add it to the list */
		column_name.push_back(string(n->u.ATTRTYPE.attrname));
		types.push_back(type);

	}
	/*cout << "������Ԫ�����ͣ�" << endl;
	for (int j = 0; j < i; ++j) {
		cout << "��" << column_name[j] << "����Ϊ��" << types[j];
		cout << endl;
	}*/
	return i;
}

/*
* mk_values: converts a single value node into a Value
*/
static void mk_value(NODE *node, Value &value)
{
	value.type = node->u.VALUE.type;
	switch (value.type) {
	case INT:
		value.data = (void *)&node->u.VALUE.ival;
		break;
	case FLOAT:
		value.data = (void *)&node->u.VALUE.rval;
		break;
	case STRING:
		value.data = (void *)node->u.VALUE.sval;
		break;
	}
}

static void mk_database(NODE* node, string& dbname) {
	if (!node) {
		dbname = "";
		return;
	}
	dbname = node->u.CREATEDATABASE.databasename;
}


/*
* mk_values: converts a list of values into an array of values
*
* Returns:
*    the lengh of the list on success ( >= 0 )
*    error code otherwise
*/
static int mk_values(NODE *list, int max, vector<int>& int_v, vector<double>& double_v, vector<string>& string_v)
{
	int i;
	int_v.clear();
	double_v.clear();
	string_v.clear();
	
	/* for each element of the list... */
	for (i = 0; list != NULL; ++i, list = list->u.LIST.next) {
		/* If the list is too long then error */
		if (i == max)
			return E_TOOMANY;
		NODE* n = list->u.LIST.curr;
		//mk_value(list->u.LIST.curr, values[i]);
		int type = n->u.VALUE.type;
		switch (type) {
		case INT:
			int_v.push_back(n->u.VALUE.ival);
			break;
		case FLOAT:
			double_v.push_back(n->u.VALUE.rval);
			break;
		case STRING:
			string_v.push_back(string(n->u.VALUE.sval));
			break;
		}
	}

	return i;
}


/*
 * mk_agg_rel_attr: converts a single relation-attribute (<relation,
 * attribute> pair) into a AggRelAttr
 * ��һ��relation-attribute��ת��ΪAggRelAttr
 */
static void mk_agg_rel_attr(NODE* node, AggRelAttr& relAttr)
{
	relAttr.func = node->u.AGGRELATTR.func;
	relAttr.relname = node->u.AGGRELATTR.relname; /* relation��ʾ��ϵ����Ӧ���ݿ��еı�,relname������ */
	relAttr.attrname = node->u.AGGRELATTR.attrname; /* attribute��ʾ���ԣ�attrname���������� */
}


/*
 * mk_agg_rel_attrs: converts a list of relation-attributes (<relation,
 * attribute> pairs) into an array of AggRelAttrs
 *
 * Returns:
 *    the lengh of the list on success ( >= 0 )
 *    error code otherwise
 */
static int mk_agg_rel_attrs(NODE* list, int max, AggRelAttr relAttrs[])
{
	int i;

	/* For each element of the list... */
	for (i = 0; list != NULL; ++i, list = list->u.LIST.next) {
		/* If the list is too long then error */
		if (i == max)
			return E_TOOMANY;

		mk_agg_rel_attr(list->u.LIST.curr, relAttrs[i]);
	}

	return i;
}


//
// mk_relations - ��list����ȡ��table������
//
static int mk_relations(NODE *list, int max, RelInfo* relations)
{
	int i;
	NODE *current;
	if (!list) {
		cout << "�ޱ�" << endl;
		return 0;
	}
	/* for each element of the list... */
	for (i = 0; list != NULL; ++i, list = list->u.LIST.next) {
		/* If the list is too long then error */
		if (i == max)
			return E_TOOMANY;

		current = list->u.LIST.curr;
		relations[i].relname=(current->u.RELATION.relname);
	}
	
	return i;
}


//
// mk_conditions - ���б�����ȡ��conditons
//
static int mk_conditions(NODE *list, int max, Condition* conditions)
{
	int i;
	NODE *current(NULL);
	if (!list) return 0;
	/* for each element of the list... */
	for (i = 0; list != NULL; ++i, list = list->u.LIST.next) {
		/* If the list is too long then error */
		if (i == max)
			return E_TOOMANY;

		current = list->u.LIST.curr;
		/* �������� -> relname.attrname op relname.attrname */
		conditions[i].lhsAttr.relname =
			current->u.CONDITION.lhsRelattr->u.RELATTR.relname;
		conditions[i].lhsAttr.attrname =
			current->u.CONDITION.lhsRelattr->u.RELATTR.attrname;
		conditions[i].op = current->u.CONDITION.op;
		if (current->u.CONDITION.rhsRelattr) { /* �Ҳ�����Ҳ������ */
			conditions[i].bRhsIsAttr = true;
			conditions[i].rhsAttr.relname =
				current->u.CONDITION.rhsRelattr->u.RELATTR.relname;
			conditions[i].rhsAttr.attrname =
				current->u.CONDITION.rhsRelattr->u.RELATTR.attrname;
		}
		else { /* �Ҳ�������ֵ */
			conditions[i].bRhsIsAttr = false;
			mk_value(current->u.CONDITION.rhsValue, conditions[i].rhsValue);
		}
	}
	return i;
}


//
// mk_rel_attr: converts a single relation-attribute (<relation, attribute> pair) into a RelAttr
//
static void mk_rel_attr(NODE *node, RelAttr &relAttr)
{
	relAttr.relname = node->u.RELATTR.relname;
	relAttr.attrname = node->u.RELATTR.attrname;
}

//
// mk_order_relattr: converts an int and a single relation-attribute (<relation, attribute> pair) into a int and a RelAttr
//
static void mk_order_relattr(NODE *node, int& order, RelAttr &relAttr)
{
	order = node->u.ORDERATTR.order;
	if (order != 0)
		mk_rel_attr(node->u.ORDERATTR.relattr, relAttr);
}
string parser_str = "parser tree: ";
int interp(NODE *n)
{
	int rc;
	RC errval = 0;
	switch (n->kind)
	{
		case N_EXIT:
			stop = true;
			break;
		case N_CREATETABLE:	/* Create Table */
		{
			int nattrs;
			string table_name(n->u.CREATETABLE.relname);
			if (table_name.size() > MAXNAME) {
				print_error("create", E_TOOLONG);
				break;
			}
			vector<int> type;
			vector<string> column_name;
			nattrs = mk_attr_infos(n->u.CREATETABLE.attrlist, MAXATTRS, column_name, type);
			if (nattrs < 0) {
				print_error("create", nattrs);
				break;
			}/*
			if (rc = dbsystem.create_table(table_name, type, column_name)) cout << "������" + table_name + "ʧ��" << endl;
			else cout << "������" + table_name + "�ɹ�" << endl;*/
			break;
		}
		case N_CREATEDATABASE:
		{
			//cout << parser_str << "create database";
			string dbname;
			mk_database(n, dbname);
			/*if (rc = dbsystem.create_database(dbname)) cout << "�������ݿ�" + dbname + "ʧ��" << endl;
			else cout << "�������ݿ�" + dbname + "�ɹ�" << endl;*/
			break;
		}
		case N_USEDATABASE:
		{
			/*string dbname(n->u.USEDATABASE.databasename);
			int rc = dbsystem.set_database(dbname);
			if (rc) cout  << "�趨���ݿ��������" << endl;
			else cout  << "��ȷ�趨���ݿ�" << dbname << endl;*/
			break;
		}
		case N_DROPTABLE:
			cout << parser_str << "drop table" << endl;
			break; 
		case N_CREATEINDEX:
		{
			string table = n->u.CREATEINDEX.relname, column = n->u.CREATEINDEX.attrname;
			/*if (rc = dbsystem.create_index(table, column, 0)) cout << "����������������" << endl;
			else cout << "�ɹ���������" << endl;*/
			//cout << n->u.CREATEINDEX.attrname << ' ' << n->u.CREATEINDEX.relname << endl;
			break;
		}
		case N_DROPINDEX:
			cout << parser_str << "drop index" << endl;
			break;
		case N_INSERT:
		{
			int nvals = 0;
			/*
			vector<int> i_v; vector<double> d_v; vector<string> s_v;
			nvals = mk_values(n->u.INSERT.valuelist, MAXATTRS, i_v, d_v, s_v);
			if (nvals < 0) {
				print_error("insert", nvals);
				break;
			}
			string table_name(n->u.INSERT.relname);
			record r(i_v, d_v, s_v);
			if (rc = dbsystem.add_record(table_name, r))
				cout << "�����¼ʧ��" << endl;
			else
				cout << "�ɹ������¼" << endl;*/
			/* Make the call to insert */
			break;
		}
		case N_LOAD:

			cout << parser_str << "load" << endl;
			break;
		case N_PRINT:
			cout << parser_str << "print" << endl;
			break;
		case N_HELP:
			if (n->u.HELP.relname)
				cout << parser_str << "help " << n->u.HELP.relname << endl;
			else
				cout << parser_str << "help" << endl;
			break;
		case N_QUERY: /* ��ѯ��� */
		{
			int nselattrs = 0;
			AggRelAttr* relAttrs = new AggRelAttr[MAXATTRS];
			int nrelations = 0; /* �����Ŀ */
			RelInfo* relations = new RelInfo[MAXATTRS]; /* ������� */
			int nconditions = 0; /* ��������Ŀ */
			Condition* conditions = new Condition[MAXCONDS];
			int order = 0; /* ����or���� */
			RelAttr orderAttr; /* ����orderAttr������ */
			bool group = false;
			RelAttr groupAttr; /* ����groupAttr������ */

			/* ��ʼ������ѡ������� */
			nselattrs = mk_agg_rel_attrs(n->u.QUERY.relattrlist, MAXATTRS, relAttrs);
			/* ��ʼ����table�������� */
			nrelations = mk_relations(n->u.QUERY.rellist, MAXATTRS, relations);
			/* ��ʼ�������� */
			nconditions = mk_conditions(n->u.QUERY.conditionlist, MAXCONDS, conditions);

			/* Make the order by attr suitable for sending to Query */
			/* ��ʼ������������ */
			mk_order_relattr(n->u.QUERY.orderrelattr, order, orderAttr);

			/* Make the group by attr suitable for sending to Query */
			mk_rel_attr(n->u.QUERY.grouprelattr, groupAttr);
			if (groupAttr.attrname != NULL)
				group = true;
			cout << "here" << endl;
			Optimizer* optimizer = new Optimizer(nrelations, relations, nselattrs, relAttrs, nconditions, conditions);

			break;
		}
		default:
			break;
	}
	return errval;
}