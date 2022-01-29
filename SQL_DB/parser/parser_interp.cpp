#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "redbase.h"
#include "../global.h"
#include "parser_node.h"
#include "parser_interp.h"
#include "../Subsystem1.h"
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
	/*cout << "创建的元素类型：" << endl;
	for (int j = 0; j < i; ++j) {
		cout << "列" << column_name[j] << "类型为：" << types[j];
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
static int mk_values(NODE *list, string RelName, char* record)
{
	int i;
	vector<Attr_Info> attrs = Subsystem1_Manager::BASE.lookup_Attrs(RelName);
	/* for each element of the list... */
	for (i = 0; list != NULL; ++i, list = list->u.LIST.next) {
		NODE* n = list->u.LIST.curr;
		//mk_value(list->u.LIST.curr, values[i]);
		int type = n->u.VALUE.type;
		
		switch (type) {
		case INT:
			memcpy(record + attrs[i].Offset, (char*)&n->u.VALUE.ival, sizeof(int));
			break;
		case FLOAT: 
			memcpy(record + attrs[i].Offset, (char*)&n->u.VALUE.rval, sizeof(float));
			break;
		case STRING:
			memcpy(record + attrs[i].Offset, (char*)&n->u.VALUE.sval, attrs[i].Length);
			break;
		}
	}

	return i;
}
static int mk_delete_cond(NODE* del, RelInfo& rel, vector<Condition>& conds) {
	rel.relname = del->u.DELETE.relname;
	NODE* list = del->u.DELETE.conditionlist;
	int i;
	for (i = 0; list; list = list->u.LIST.next, ++i) {
		Condition cond;
		NODE* cond_node = list->u.LIST.curr;
		cond.lhsAttr.relname = cond_node->u.CONDITION.lhsRelattr->u.RELATTR.relname;
		cond.lhsAttr.attrname = cond_node->u.CONDITION.lhsRelattr->u.RELATTR.attrname;
		cond.op = cond_node->u.CONDITION.op;
		if (!cond_node->u.CONDITION.rhsRelattr) {
			cond.bRhsIsAttr = false;
			cond.rhsValue.type = cond_node->u.CONDITION.rhsValue->u.VALUE.type;
			switch (cond.rhsValue.type)
			{
			case INT:
				cond.rhsValue.data = (void*)&cond_node->u.CONDITION.rhsValue->u.VALUE.ival;
				break;
			case FLOAT:
				cond.rhsValue.data = (void*)&cond_node->u.CONDITION.rhsValue->u.VALUE.rval;
				break;
			case STRING:
				cond.rhsValue.data = (void*)cond_node->u.CONDITION.rhsValue->u.VALUE.sval;
				break;
			}
		}
		else {
			cond.bRhsIsAttr = true;
			cond.rhsAttr.attrname = cond_node->u.CONDITION.rhsRelattr->u.RELATTR.attrname;
			cond.rhsAttr.relname = cond_node->u.CONDITION.lhsRelattr->u.RELATTR.relname;
		}
		
		conds.push_back(cond);
	}
	return i;
}

static int mk_update_cond(NODE* update, RelInfo& rel, vector<Condition>& conds) {
	rel.relname = update->u.UPDATE.relname;
	NODE* list = update->u.UPDATE.conditionlist;
	int i;
	for (i = 0; list; list = list->u.LIST.next, ++i) {
		Condition cond;
		NODE* cond_node = list->u.LIST.curr;
		cond.lhsAttr.relname = cond_node->u.CONDITION.lhsRelattr->u.RELATTR.relname;
		cond.lhsAttr.attrname = cond_node->u.CONDITION.lhsRelattr->u.RELATTR.attrname;
		cond.op = cond_node->u.CONDITION.op;
		if (!cond_node->u.CONDITION.rhsRelattr) {
			cond.bRhsIsAttr = false;
			cond.rhsValue.type = cond_node->u.CONDITION.rhsValue->u.VALUE.type;
			switch (cond.rhsValue.type)
			{
			case INT:
				cond.rhsValue.data = (void*)&cond_node->u.CONDITION.rhsValue->u.VALUE.ival;
				break;
			case FLOAT:
				cond.rhsValue.data = (void*)&cond_node->u.CONDITION.rhsValue->u.VALUE.rval;
				break;
			case STRING:
				cond.rhsValue.data = (void*)cond_node->u.CONDITION.rhsValue->u.VALUE.sval;
				break;
			}
		}
		else {
			cond.bRhsIsAttr = true;
			cond.rhsAttr.attrname = cond_node->u.CONDITION.rhsRelattr->u.RELATTR.attrname;
			cond.rhsAttr.relname = cond_node->u.CONDITION.lhsRelattr->u.RELATTR.relname;
		}

		conds.push_back(cond);
	}
	return i;
}

/*
 * mk_agg_rel_attr: converts a single relation-attribute (<relation,
 * attribute> pair) into a AggRelAttr
 * 将一个relation-attribute对转化为AggRelAttr
 */
static void mk_agg_rel_attr(NODE* node, AggRelAttr& relAttr)
{
	relAttr.func = node->u.AGGRELATTR.func;
	relAttr.relname = node->u.AGGRELATTR.relname; /* relation表示关系，对应数据库中的表,relname即表名 */
	relAttr.attrname = node->u.AGGRELATTR.attrname; /* attribute表示属性，attrname即属性名称 */
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
// mk_relations - 从list中提取出table的名称
//
static int mk_relations(NODE *list, int max, RelInfo* relations)
{
	int i;
	NODE *current;
	if (!list) {
		cout << "无表" << endl;
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
// mk_conditions - 从列表中提取出conditons
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
		/* 条件构成 -> relname.attrname op relname.attrname */
		conditions[i].lhsAttr.relname =
			current->u.CONDITION.lhsRelattr->u.RELATTR.relname;
		conditions[i].lhsAttr.attrname =
			current->u.CONDITION.lhsRelattr->u.RELATTR.attrname;
		conditions[i].op = current->u.CONDITION.op;
		if (current->u.CONDITION.rhsRelattr) { /* 右操作数也是属性 */
			conditions[i].bRhsIsAttr = true;
			conditions[i].rhsAttr.relname =
				current->u.CONDITION.rhsRelattr->u.RELATTR.relname;
			conditions[i].rhsAttr.attrname =
				current->u.CONDITION.rhsRelattr->u.RELATTR.attrname;
		}
		else { /* 右操作数是值 */
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
//解析update的新值
static int mk_update_new_val(NODE* node, NODE*& attrs, char**& new_val) {
	NODE* list = node;
	vector<NODE*> attr;
	vector<char*> value;
	while (list) {
		attr.push_back(list->u.LIST.curr->u.UPDATE_NEW_VAL.attr);
		value.push_back(list->u.LIST.curr->u.UPDATE_NEW_VAL.new_val);
		list = list->u.LIST.next;
	}
	attrs = new NODE[attr.size()];
	new_val = new char*[attr.size()];
	for (int i = 0; i < attr.size(); ++i) attrs[i] = *attr[i];
	for (int i = 0; i < attr.size(); ++i) new_val[i] = value[i];
	return attr.size();
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
			if (rc = dbsystem.create_table(table_name, type, column_name)) cout << "创建表" + table_name + "失败" << endl;
			else cout << "创建表" + table_name + "成功" << endl;*/
			break;
		}
		case N_CREATEDATABASE:
		{
			//cout << parser_str << "create database";
			string dbname;
			mk_database(n, dbname);
			/*if (rc = dbsystem.create_database(dbname)) cout << "创建数据库" + dbname + "失败" << endl;
			else cout << "创建数据库" + dbname + "成功" << endl;*/
			break;
		}
		case N_USEDATABASE:
		{
			/*string dbname(n->u.USEDATABASE.databasename);
			int rc = dbsystem.set_database(dbname);
			if (rc) cout  << "设定数据库出现问题" << endl;
			else cout  << "正确设定数据库" << dbname << endl;*/
			break;
		}
		case N_DROPTABLE:
			cout << parser_str << "drop table" << endl;
			break; 
		case N_CREATEINDEX:
		{
			string table = n->u.CREATEINDEX.relname, column = n->u.CREATEINDEX.attrname;
			/*if (rc = dbsystem.create_index(table, column, 0)) cout << "创建索引出现问题" << endl;
			else cout << "成功创建索引" << endl;*/
			//cout << n->u.CREATEINDEX.attrname << ' ' << n->u.CREATEINDEX.relname << endl;
			break;
		}
		case N_DROPINDEX:
			cout << parser_str << "drop index" << endl;
			break;
		case N_INSERT:
		{
			int nvals = 0;
			string rel_name = n->u.INSERT.relname;
			char record[500];
			nvals = mk_values(n->u.INSERT.valuelist, rel_name, record);
			Rel_Info rel_info;
			RelInfo rel;
			Subsystem1_Manager::BASE.lookup_Rel(rel_name, rel_info);
			rel.relname = rel_info.Rel_Name;
			Optimizer* optimizer = new Optimizer(rel, record);
			/* Make the call to insert */
			break;
		}
		case N_DELETE: {
			RelInfo rel;
			vector<Condition> conds;
			int cond_num = mk_delete_cond(n, rel, conds);
			Condition* conds_arr = new Condition[cond_num];
			for (int i = 0; i < cond_num; ++i) conds_arr[i] = conds[i];
			Optimizer* optimizer = new Optimizer(rel, cond_num, conds_arr);
			break;
		}
		case N_UPDATE: {
			//Optimizer(RelInfo rel, int Attr_num, AggRelAttr* attrs, char** values, 
			//int Cond_num, Condition* conds, SQL_type sql_type)
			RelInfo rel;
			int Attr_num; NODE* attrs; char** values;
			vector<Condition> conds;
			Attr_num = mk_update_new_val(n->u.UPDATE.new_val, attrs, values);
			int cond_num = mk_update_cond(n, rel, conds);
			Condition* conds_arr = new Condition[cond_num];
			for (int i = 0; i < cond_num; ++i) conds_arr[i] = conds[i];
			Optimizer* optimizer = new  Optimizer(rel, Attr_num, attrs, values, cond_num, conds_arr);
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
		case N_QUERY: /* 查询语句 */
		{
			int nselattrs = 0;
			AggRelAttr* relAttrs = new AggRelAttr[MAXATTRS];
			int nrelations = 0; /* 表的数目 */
			RelInfo* relations = new RelInfo[MAXATTRS]; /* 表的名称 */
			int nconditions = 0; /* 条件的数目 */
			Condition* conditions = new Condition[MAXCONDS];
			int order = 0; /* 升序or降序 */
			RelAttr orderAttr; /* 按照orderAttr来排序 */
			bool group = false;
			RelAttr groupAttr; /* 按照groupAttr来分组 */

			/* 开始解析所选择的属性 */
			nselattrs = mk_agg_rel_attrs(n->u.QUERY.relattrlist, MAXATTRS, relAttrs);
			/* 开始解析table的名称了 */
			nrelations = mk_relations(n->u.QUERY.rellist, MAXATTRS, relations);
			/* 开始解析条件 */
			nconditions = mk_conditions(n->u.QUERY.conditionlist, MAXCONDS, conditions);

			/* Make the order by attr suitable for sending to Query */
			/* 开始解析排序属性 */
			mk_order_relattr(n->u.QUERY.orderrelattr, order, orderAttr);

			/* Make the group by attr suitable for sending to Query */
			mk_rel_attr(n->u.QUERY.grouprelattr, groupAttr);
			if (groupAttr.attrname != NULL)
				group = true;

			
			Optimizer* optimizer = new Optimizer(nrelations, relations, nselattrs, relAttrs, nconditions, conditions);
			
			break;
		}
		default:
			break;
	}
	return errval;
}