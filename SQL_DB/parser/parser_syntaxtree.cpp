#include "parser_syntaxtree.h"
#include "parser_node.h"
//#include "DBstruct.h"

//
// buildSyntaxTree - ����һ���﷨��
//
NODE* SyntaxTree::buildSyntaxTree()
{
	NODE *node = nullptr;
	bool succ = parseCommand(node);
	if (!succ) succ = parseUtility(node);
	if (succ) {
		TokenPtr ta = next();
		if (ta->type == RW_SEMICOLON) return node;
		throw GeneralError("Command should end with ;");
	}
	throw GeneralError("Not supported command");
	return node;
}

//
// parseCommand - ��������
//	command -> ddl | dml
//
bool SyntaxTree::parseCommand(NODE* &node)
{
	bool succ = parseDDL(node);
	if (!succ) succ = parseDML(node);
	return succ;
}

//
// parseUtility - ������������
//	utility -> load RW_STRING RW_LPAREN T_QSTRING RW_RPAREN
//			-> RW_EXIT
//			-> RW_HELP RW_STRING | RW_HELP
//			-> RW_PRINT RW_STRING
//			-> RW_SET RW_STRING
//
bool SyntaxTree::parseUtility(NODE* &node)
{
	TokenPtr ta = peek(1);
	TokenPtr tb, tc;
	if (!ta) return false;
	switch (ta->type)
	{
	case RW_LOAD: 
	{
		discard(1);
		ta = next();
		if (!ta || ta->type != RW_STRING) goto tg;
		tb = next();
		if (!tb || tb->type != RW_LPAREN) goto tg;
		tb = next();
		if (!tb || tb->type != T_QSTRING) goto tg;
		tc = next();
		if (!tc || tc->type != RW_RPAREN) goto tg;
		node = load_node(ta->content, tb->content);
		return true;
	}
	case RW_EXIT:
	{
		discard(1);
		node = exit_node();
		return true;
	}
	case RW_HELP:
	{
		discard(1);
		ta = peek(1);
		if (!ta) goto tg;
		if (ta->type != RW_STRING)
			node = help_node(nullptr);
		else {
			discard(1);
			node = help_node(ta->content);
		}
		return true;
	}
	case RW_PRINT:
	{
		discard(1);
		ta = next();
		if (!ta || ta->type != RW_STRING) goto tg;
		node = print_node(ta->content);
		return true;
	}
	case RW_SET:
	{
		discard(1);
		ta = next();
		if (!ta || ta->type != RW_STRING) goto tg;
		tb = next();
		if (!tb || tb->type != T_EQ) goto tg;
		tb = next();
		if (!tb || tb->type != RW_STRING) goto tg;
		node = set_node(ta->content, tb->content);
		return true;
	}
	default:
		break;
	}
	return false;
tg:
	throw GeneralError("Utility Command error!");
}

//
// parseDDL - �������ݿⶨ������
//	ddl -> createtable | createindex | droptable | dropindex
//
bool SyntaxTree::parseDDL(NODE* &node)
{
	bool succ = parseCreateTable(node);
	if (!succ) succ = parseCreateDatabase(node);
	if (!succ) succ = parseUseDatabase(node);
	if (!succ) succ = parseCreateIndex(node);
	if (!node) succ = parseDropTable(node);
	if (!node) succ = parseDropIndex(node);
	return succ;
}

//
// parseDML - ����dml
// dml -> query | insert
//
bool SyntaxTree::parseDML(NODE* &node)
{
	bool succ = parseQuery(node);
	if (!succ) succ = parseInsert(node);
	if (!succ) succ = parseDelete(node);
	if (!succ) succ = parseUpdate(node);
	return succ;
}

//
// parseQuery - ����select���
//  query -> RW_SELECT non_mt_select_clause RW_FROM non_mt_relation_list opt_where_clause 
//				opt_order_by_clause opt_group_by_clause
//		  -> RW_SELECT non_mt_select_clause RW_FROM non_mt_relation_list opt_where_clause
//				opt_group_by_clause opt_order_by_clause
//
bool SyntaxTree::parseQuery(NODE* &node)
{
	TokenPtr ta = peek(1);
	if (ta && ta->type == RW_SELECT) {
		discard(1);
		NODE* clause(NULL);
		parseNonmtSelectClause(clause);
		ta = next();
		if (!ta || ta->type != RW_FROM) goto tg;
		NODE* relattrlist(NULL), *condlist(NULL), *groupby(NULL), *orderby(NULL);
		parseNonmtRelationList(relattrlist);
		parseOptWhereClause(condlist);
		/* order by��group by���Խ���λ�� */
		parseOptOrderByClause(orderby);
		parseOptGroupByClause(groupby);
		if (orderby == nullptr && groupby != nullptr) {
			parseOptOrderByClause(orderby);
		}
		node = query_node(clause, relattrlist, condlist, orderby, groupby);
		return true;
	}
	node == nullptr;
	return false;
	tg:
	throw GeneralError("Something wrong with the select. Please check your input.");
}

//
// parseOptGroupByClause - ����group by���
//  opt_group_by_caluse -> nothing | RW_GROUP RW_BY relattr 
//
void SyntaxTree::parseOptGroupByClause(NODE* &node)
{
	TokenPtr ta = peek(1);
	if (ta && ta->type == RW_GROUP) {
		discard(1);
		ta = next();
		if (!ta || ta->type != RW_BY) goto tg;
		bool succ = parseRelAttr(node);
		if (!succ) goto tg;
		return;
	}
	node = relattr_node(nullptr, nullptr);
	return;
tg:
	throw GeneralError("Something wrong with the group by sentences.");
}

//
// parseOptOrderByClause -����order by���
//  opt_order_by_clause -> RW_ORDER RW_BY ordering_spec
//						-> nothing
//  ordering_spec -> relattr opt_asc_desc
//	opt_asc_desc -> RW_DESC | RW_ASC
//
void SyntaxTree::parseOptOrderByClause(NODE* &node)
{
	TokenPtr ta = peek(1), tb;
	if (ta && ta->type == RW_ORDER) {
		discard(1);
		ta = next();
		if (!ta && ta->type != RW_BY) goto tg;
		NODE* relattr;
		if (!parseRelAttr(relattr)) goto tg;
		ta = peek(1);
		if (!ta)  goto tg;
		int order;
		switch (ta->type)
		{
			case RW_DESC: discard(1); order = -1; break;
			case RW_ASC: discard(1);  order = 1; break;
			default: order = 1; break;
		}
		node = orderattr_node(order, relattr); 
		return;
	}
	node = orderattr_node(0, 0);
	return;
tg:
	throw GeneralError("Something wrong with the order by sentences.");
}

//
// parseOptWhereCluase - ����where�����������,��������ܻ�ɹ�.
//  opt_where_cluase -> RW_WHERE non_mt_cond_list
//					-> nothing
void SyntaxTree::parseOptWhereClause(NODE* &node)
{
	TokenPtr t = peek(1);
	if (t && t->type == RW_WHERE) {
		discard(1);
		parseNonmtCondList(node);
		return;
	}
	node == nullptr;
}

//
// parseNonmtCondList - ����where������������
//	non_mt_cond_list -> condition RW_AND non_mt_cond_list
//					-> condition
void SyntaxTree::parseNonmtCondList(NODE* &node)
{
	NODE* condlist, *cond;
	parseCondition(cond);
	TokenPtr ta = peek(1);
	if (ta) {
		if (ta->type == RW_AND) {
			discard(1);
			parseNonmtCondList(condlist);
			node = prepend(cond, condlist);
		}
		else node = list_node(cond);
		return;
	}
	throw GeneralError("Something wrong with the condlist.");
}

//
// parseCondition - ����where������������
//	condition -> relattr op relattr_or_value
//
void SyntaxTree::parseCondition(NODE* &node)
{
	bool succ = parseRelAttr(node);
	NODE* rel_or_val;
	if (succ) {
		TokenPtr tk = peek(1);
		int op = tk->type;
		discard(1);
		
		parseRelAttrOrValue(rel_or_val);
		node = condition_node(node, op, rel_or_val);
	}
	else
		throw GeneralError("Something wrong with the condition sentences.");
}

//
// parseRelAttrOrValue - �������Ի���ֵ
//  relattr_or_value - relattr | value
//
void SyntaxTree::parseRelAttrOrValue(NODE* &node)
{
	if (parseRelAttr(node)) {
		node = relattr_or_value_node(node, nullptr);
	}
	else if (parseValue(node)){
		node = relattr_or_value_node(nullptr, node);
	}
	else {
		throw GeneralError("Something wrong with the relation attr or value.");
	}
}

//
// parseRelAttr - �����������
//  relattr -> RW_STRING RW_DOT RW_STRING
//			-> RW_STRING
//
bool SyntaxTree::parseRelAttr(NODE* &node)
{
	TokenPtr ta = peek(1), tb;
	if (ta && ta->type == RW_STRING) {
		discard(1);
		tb = peek(1);
		if (!tb) goto tg;
		if (tb->type == RW_DOT) {
			discard(1);
			tb = next();
			if (!tb || tb->type != RW_STRING) goto tg;
			node = relattr_node(ta->content, tb->content);
		}
		else
			node = relattr_node(nullptr, ta->content);
		return true;
	}
	node = nullptr;
	return false;
tg:
	throw GeneralError("Something wrong with the relation attributions.");
}

//
// parseNonmtRelationList - ����table�������б�
//  non_mt_relation_list -> relation RW_COMMA non_mt_relation_list
//						 -> relation
//	relation -> RW_STRING
void SyntaxTree::parseNonmtRelationList(NODE* &node)
{
	TokenPtr ta = next(), tb;
	NODE* rel;
	if (!ta || ta->type != RW_STRING) goto tag;
	rel = relation_node(ta->content);
	tb = peek(1);
	if (!tb) goto tag;
	if (tb->type != RW_COMMA) node = list_node(rel);
	else {
		discard(1);
		NODE* rellist;
		parseNonmtRelationList(rellist);
		node = prepend(rel, rellist);
	}
	return;
tag:
	throw GeneralError("Something wrong with the table name.Please Check the sentences.");
}

//
// parseNonmtSelectClause - ����select��������clause
// non_mt_select_clause -> non_mt_aggrelattr_list
//						->	RW_STAR
// 
void SyntaxTree::parseNonmtSelectClause(NODE* &node)
{
	TokenPtr ta = peek(1);
	if (ta) {
		if (ta->type == RW_STAR) {
			discard(1);
			node = list_node(aggrelattr_node(NO_F, NULL, (char *)"*"));
		}
		else
			parseNonmtAggrelattrList(node);
		return;
	}
	throw GeneralError("Select clause not right!");
}

//
// parseNonmtAggrelattrList - ���������б�
//	non_mt_aggrelatr_list -> aggrelattr RW_COMMA non_mt_aggrelattr_list
//						  -> aggrelattr
//
void SyntaxTree::parseNonmtAggrelattrList(NODE* &node)
{
	bool succ = parseAggrelattr(node);
	TokenPtr t = peek(1);
	if (t) {
		if (t->type == RW_COMMA) {
			discard(1);
			NODE* attrlist;
			parseNonmtAggrelattrList(attrlist);
			node = prepend(node, attrlist);
		}
		else {
			node = list_node(node);
		}
		return;
	}
	throw GeneralError("Aggrelattr list is not right!");
}
bool SyntaxTree::parseCreateDatabase(NODE*& node) {
	TokenPtr ta = peek(1);
	TokenPtr tb = peek(2);
	NODE* dbName(NULL);
	if (ta && tb && ta->type == RW_CREATE && tb->type == RW_DATABASE) {
		discard(2);
		ta = next();
		if (!ta || ta->type != RW_STRING) goto tg;
		node = create_database_node(ta->content);

		return true;
	}
	return false;
tg:
	throw GeneralError("�������ݿ�������");
}
bool SyntaxTree::parseCreateTable(NODE* &node)
{
	TokenPtr ta = peek(1);
	TokenPtr tb = peek(2);
	NODE* attrlist(NULL);
	if (ta && tb && ta->type == RW_CREATE && tb->type == RW_TABLE) {
		discard(2);
		ta = next();
		if (!ta || ta->type != RW_STRING) goto tg;
		tb = next();
		if (!tb || tb->type != RW_LPAREN) goto tg;
		parseNonmtAttrtypeList(attrlist);
		if (!attrlist) goto tg;		// attrlist����Ϊ��
		tb = next();
		if (!tb || tb->type != RW_RPAREN) goto tg;
		node = create_table_node(ta->content, attrlist);
		return true;
	}
	return false;
tg:
	throw GeneralError("Please check the Create Table grammar, Your input is not right.");
}

bool SyntaxTree::parseUseDatabase(NODE*& node) {
	TokenPtr ta = peek(1);
	if (ta && ta->type == RW_USE) {
		discard(1);
		ta = next();
		if (!ta || ta->type != RW_STRING) goto tg;
		node = use_database_node(ta->content);
		return true;
	}	
	return false;
tg:
	throw GeneralError("����use databaseʱ����,����!");
}

bool SyntaxTree::parseCreateIndex(NODE* &node)
{
	TokenPtr ta = peek(1);
	TokenPtr tb = peek(2);
	TokenPtr tc;
	if (ta && tb && ta->type == RW_CREATE && tb->type == RW_INDEX) {
		discard(2);
		ta = next();
		if (!ta || ta->type != RW_STRING) goto tg;
		tb = next();
		if (!tb || tb->type != RW_LPAREN) goto tg;
		tb = next();
		if (!tb || tb->type != RW_STRING) goto tg;
		tc = next();
		if (!tc || tc->type != RW_RPAREN) goto tg;
		node = create_index_node(ta->content, tb->content);
		return true;
	}
	return false;
tg:
	throw GeneralError("����create indexʱ����,����!");
}
/*��delete���Ľ���*/
bool SyntaxTree::parseDelete(NODE*& node) {
	TokenPtr ta = peek(1);
	TokenPtr tb = peek(2);
	NODE* condlist = NULL;
	if (ta && tb && ta->type == RW_DELETE && tb->type == RW_FROM) {
		discard(2);
		ta = next();
		if (!ta || ta->type != RW_STRING) goto tg;
		parseOptWhereClause(condlist);
		node = delete_node(ta->content, condlist);
		return true;
	}
	node = NULL;
	return false;
tg:
	throw GeneralError("Please check the sentence. Something wrong with the \'delete from\'.");
}
/*��update���Ľ�����δ�깤*/
bool SyntaxTree::parseUpdate(NODE*& node) {
	TokenPtr ta = peek(1), tb;
	NODE* condlist, *new_val;
	if (ta && ta->type == RW_UPDATE) {
		discard(1);
		//ta is the name of relation 
		ta = next();
		if (!ta || ta->type != RW_STRING) goto tg;
		tb = next();
		if (!tb || tb->type != RW_SET) goto tg;
		parseUpdateNewValue(new_val);
		tb = peek(1);
		if (!tb) {
			node = update_node(ta->content, new_val, NULL);
			return true;
		}
		parseOptWhereClause(condlist);
		node = update_node(ta->content, new_val, condlist);
		return true;
	}
	node = NULL;
	return false;
tg:
	throw GeneralError("Please check the sentence. Something wrong with the \'delete from\'.");
}
//
// parseInsert - ����insert�﷨
//	insert -> RW_INSERT RW_INTO RW_STRING RW_LPAREN non_mt_value_list RW_RPAREN
//
bool SyntaxTree::parseInsert(NODE* &node)
{
	TokenPtr ta = peek(1);
	TokenPtr tb = peek(2);
	NODE* vallist;
	if (ta && tb && ta->type == RW_INSERT && tb->type == RW_INTO) {
		discard(2);
		ta = next();
		if (!ta || ta->type != RW_STRING) goto tg;

		tb = next();
		if (!tb || tb->type != RW_LPAREN) goto tg;
		parseNonmtValueList(vallist);
		tb = next();
		if (!tb || tb->type != RW_RPAREN) goto tg;
		node = insert_node(ta->content, vallist);
		return true;
	}
	node = nullptr;
	return false;
tg:
	throw GeneralError("Please check the sentence. Something wrong with the \'insert into\'.");
}

//
// parseAggrelattr - ���������б�
//	aggrelattr -> ammsc RW_LPAREN RW_STRING RW_DOT RW_STRING RW_RPAREN
//				-> ammsc RW_LPAREN RW_STRING RW_RPAREN
//				-> ammsc RW_LPAREN RW_STAR RW_RPAREN
//				-> RW_STRING RW_DOT RW_STRING
//				-> RW_STRING
//  ammsc -> RW_AVG | RW_MAX | RW_MIN | RW_SUM | RW_COUNT;
//
bool SyntaxTree::parseAggrelattr(NODE* &node)
{
	TokenPtr ta = peek(1), tb, tc;
	if (ta) {
		if (ta->type != RW_STRING) {
			discard(1);
			AggFun fun;
			switch (ta->type)
			{
			case RW_AVG: fun = AVG_F; break;
			case RW_MAX: fun = MAX_F; break;
			case RW_MIN: fun = MIN_F; break;
			case RW_SUM: fun = SUM_F; break;
			case RW_COUNT: fun = COUNT_F; break;
			default:
				throw GeneralError("Something wrong with the aggrelatter");
			}
			ta = next();
			if (!ta || ta->type != RW_LPAREN) goto tg;
			ta = next();
			if (!ta) goto tg;
			if (ta->type == RW_STAR) {
				ta = next();
				if (!ta || ta->type != RW_RPAREN) goto tg;
				node = aggrelattr_node(fun, NULL, (char *)"*");
				return true;
			}
			if (ta->type != RW_STRING) goto tg;
			tb = next();
			if (!tb) goto tg;
			if (tb->type == RW_RPAREN) {
				node = aggrelattr_node(fun, NULL, ta->content);
				return true;
			}
			if (tb->type != RW_DOT) goto tg;
			tb = next();
			if (!tb || tb->type != RW_STRING) goto tg;
			tc = next();
			if (!tc || tc->type != RW_RPAREN) goto tg;
			node = aggrelattr_node(fun, ta->content, tb->content);
		}
		else {
			discard(1);
			tb = peek(1);
			if (!tb) goto tg;
			if (tb->type != RW_DOT) {
				node = aggrelattr_node(NO_F, nullptr, ta->content);
				return true;
			}
			else {
				discard(1); /* ���ĵ�RW_DOT */
				tb = next();
				if (!tb || tb->type != RW_STRING) goto tg;
				node = aggrelattr_node(NO_F, ta->content, tb->content);
				return true;
			}
		}
	}
tg:
	throw GeneralError("Something wrong with the function");
}

//
// parseNonmtValueList - ����ֵ�б�
//  non_mt_value_list -> value RW_COMMA non_mt_value_list
//					  -> value
//
bool SyntaxTree::parseNonmtValueList(NODE* &node)
{
	NODE* val(NULL);
	parseValue(val);
	TokenPtr ta = peek(1);
	if (ta) {
		if (ta->type == RW_COMMA) {
			discard(1);
			NODE* vallist;
			parseNonmtValueList(vallist);
			node = prepend(val, vallist);
		}
		else node = list_node(val);
		return true;
	}
	throw GeneralError("Value list could not be empty!");
}

//
// parseValue - ����ֵ
//	value -> T_STRING | T_INT | T_FLOAT
//
bool SyntaxTree::parseValue(NODE* &node)
{
	TokenPtr val = next();
	switch (val->type) {
	case T_QSTRING:
		node = value_node(STRING, val->content);
		break;
	case T_INT:
	{
		int v=0;
		sscanf_s(val->content, "%d", &v);
		node = value_node(INT, &v);
		break;
	}
	case T_FLOAT:
	{
		float v;
		sscanf_s(val->content, "%f", &v);
		node = value_node(FLOAT, &v);
		break;
	}
	default:
		node = nullptr;
		return false;
	}
	return true;
}

//
// parseDropTable - ����drop table���﷨
//	droptable -> RW_DROP RW_TABLE RW_STRING
//
bool SyntaxTree::parseDropTable(NODE* &node)
{
	TokenPtr ta = peek(1);
	TokenPtr tb = peek(2);
	TokenPtr tc;
	if (ta && tb && ta->type == RW_DROP && tb->type == RW_TABLE) {
		discard(2);
		ta = next();
		if (!ta || ta->type != RW_STRING) goto tg;
		node = drop_table_node(ta->content);
		return true;
	}
	node = nullptr;
	return false;
tg:
	throw GeneralError("����drop tableʱ����,����!");
}

//
// parseDropIndex - ����drop index�﷨
//  dropindex -> RW_DROP RW_INDEX RW_STRING RW_LPAREN RW_STRING RW_RPAREN 
//
bool SyntaxTree::parseDropIndex(NODE* &node)
{
	TokenPtr ta = peek(1);
	TokenPtr tb = peek(2);
	TokenPtr tc;
	if (ta && tb && ta->type == RW_DROP && tb->type == RW_INDEX) {
		discard(2);
		ta = next();
		if (!ta || ta->type != RW_STRING) goto tg;
		tb = next();
		if (!tb || tb->type != RW_LPAREN) goto tg;
		tb = next();
		if (!tb || tb->type != RW_STRING) goto tg;
		tc = next();
		if (!tc || tc->type != RW_RPAREN) goto tg;
		node = drop_index_node(ta->content, tb->content);
		return true;
	}
	return false;
tg:
	throw GeneralError("drop index���д��,����ϸ���!");
}


bool SyntaxTree::parseNonmtAttrtypeList(NODE* &node)
{
	NODE* attr(NULL);
	parseAttrtype(attr);
	TokenPtr ta = peek(1);
	if (attr && ta) {
		if (ta->type == RW_COMMA) {
			discard(1);
			NODE* attrlist;
			parseNonmtAttrtypeList(attrlist);
			node = prepend(attr, attrlist);
		}
		else {
			node = list_node(attr);
		}
		return true;
	}
	throw GeneralError("AttrType list could not be empty!");
}

bool SyntaxTree::parseAttrtype(NODE* &node)
{
	TokenPtr ta = next();
	TokenPtr tb = next();
	if (ta && tb && ta->type == RW_STRING && tb->type == RW_STRING) {
		node = attrtype_node(ta->content, tb->content);
		return true;
	}
	throw GeneralError("Attrtype is wrong. Please check the sentence!");
}
//����update set�������ָ������
bool SyntaxTree::parseUpdateNewValue(NODE*& node) {
	node = newnode(NODEKIND::N_LIST);
	NODE* list = node;
	TokenPtr ta, tb, tc;
	while (true) {
		ta = next();
		tb = next();
		tc = next();
		if (!ta || !tb || !tc || ta->type != RW_STRING || tb->type != RW_DOT) return false;
		tb = next();
		if (!tb || tb->type != T_EQ) return false;
		tb = next();
		list->u.LIST.curr = update_new_val_node(ta->content, tc->content, tb->content);
		list->u.LIST.next = NULL;
		tb = peek(1);
		if (!tb || tb->type != RW_COMMA) return node;
		discard(1);
		list->u.LIST.next = newnode(NODEKIND::N_LIST);
		list = list->u.LIST.next;
	}
	throw GeneralError("��������ֵ��������");
}



//
// next - ��ȡ��һ��token
// 
TokenPtr SyntaxTree::next()
{
	TokenPtr t;
	if (tokens_.size() != 0) t = tokens_.front();
	else return lexer_->next();
	tokens_.pop_front();
	return t;
}

//
// peek - ��ǰ͵��,��Ϊ�����ڲ�ʹ�õĺ���,���Բ���Բ������м��
//
TokenPtr SyntaxTree::peek(int pos)
{
	while (tokens_.size() < pos) {
		TokenPtr t = lexer_->next();
		if (t == nullptr) return nullptr;
		tokens_.push_back(t);
	}
	return tokens_[pos - 1];
}

//
// discard - ����ָ��������token
// 
void SyntaxTree::discard(int num)
{
	while (tokens_.size() != 0 && num != 0) {
		tokens_.pop_front();
		num--;
	}
	while (num > 0) {
		lexer_->next();
		num--;
	}
}


