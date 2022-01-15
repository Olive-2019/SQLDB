#include "Query_Executor.h"
#include "../Subsystem1.h"
#include "../parser/parser_token.h"
Query_Executor::Query_Executor(Logical_TreeNode* Root)
{
	this->Root = Root;
}

vector<RID> Query_Executor::query()
{
	return execute_tree_node(Root);
}

vector<RID> Query_Executor::execute_tree_node(Logical_TreeNode* node)
{
	vector<RID> ret;
	if (node->kind == Logical_TreeNode_Kind::PLAN_FILESCAN) {  //无条件的叶节点
		return execute_tree_node_scan(node);
	}

	else if (node->kind == Logical_TreeNode_Kind::PLAN_FILTER) {
		execute_tree_node_filter(node);
	}

	else if (node->kind == Logical_TreeNode_Kind::PLAN_JOIN) {
		return execute_tree_node_join(node);
	}

	else if (node->kind == Logical_TreeNode_Kind::PLAN_PROJ) {
		return execute_tree_node_proj(node);
	}

}
vector<RID> Query_Executor::execute_tree_node_scan(Logical_TreeNode* node, vector<Condition*> cond)
{
	node->RelName = node->u.FILESCAN.Rel;


	vector<RID> ret;
	vector<Attr_Info> attrs = Subsystem1_Manager::BASE.lookup_Attrs(node->u.FILESCAN.Rel);
	string key;
	bool hasIndex = false;
	Index_Info index;
	Condition* key_cond = NULL;
	auto it = cond.begin();
	for (int i = 0; i < cond.size(); ++i) {
		it++;
		key = ((Condition*)cond[i])->lhsAttr.attrname;
		key_cond = (Condition*)cond[i];
		//不等于不能用索引
		if (key_cond[i].op == TOKENKIND::T_NE) continue;
		hasIndex = Subsystem1_Manager::BASE.lookup_Index(node->u.FILESCAN.Rel, key, index);
		if (hasIndex) {
			cond.erase(it);
			break;
		}
	}
	if (hasIndex) {
		Index_Reader* reader = NULL;
		//针对当前索引获取
		switch (key_cond->op)
		{
		case TOKENKIND::T_EQ:
			reader = new Index_Reader(node->u.FILESCAN.Rel, index, true, (char*)key_cond->rhsValue.data, true, (char*)key_cond->rhsValue.data);
			break;
		case TOKENKIND::T_GE:
			reader = new Index_Reader(node->u.FILESCAN.Rel, index, true, (char*)key_cond->rhsValue.data, true, NULL);
			break;
		case TOKENKIND::T_GT:
			reader = new Index_Reader(node->u.FILESCAN.Rel, index, false, (char*)key_cond->rhsValue.data, true, NULL);
			break;
		case TOKENKIND::T_LE:
			reader = new Index_Reader(node->u.FILESCAN.Rel, index, true, NULL, true, (char*)key_cond->rhsValue.data);
			break;
		case TOKENKIND::T_LT:
			reader = new Index_Reader(node->u.FILESCAN.Rel, index, true, NULL, false, (char*)key_cond->rhsValue.data);
			break;
		default:
			break;
		}

		RID temp;
		char* record = NULL;
		attrs = Subsystem1_Manager::BASE.lookup_Attrs(node->u.FILESCAN.Rel);
		while ((record = reader->get_Next_Record_with_RID(temp)) != NULL) {
			for (int i = 0; i < cond.size(); ++i)
				if (!record_cmp((Condition*)cond[i], record, attrs)) continue;
			ret.push_back(temp);
		}
		return ret;
	}
	Reader* reader = new Scan_Reader(node->u.FILESCAN.Rel);
	RID temp;
	char* record = NULL;
	attrs = Subsystem1_Manager::BASE.lookup_Attrs(node->u.FILESCAN.Rel);
	while ((record = reader->get_Next_Record_with_RID(temp)) != NULL) {
		for (int i = 0; i < cond.size(); ++i)
			if (!record_cmp((Condition*)cond[i], record, attrs)) continue;
		ret.push_back(temp);
	}
	cond.clear();
	return ret;
}
vector<RID> Query_Executor::execute_tree_node_filter(Logical_TreeNode* node)
{
	vector<RID> ret;
	vector<Condition*> Conds;
	Conds.push_back(node->u.FILTER.expr_filter);//一元条件落到叶结点执行，二元条件亦可
	ret = execute_tree_node_under_filter(node->u.FILTER.rel, Conds);
	node->RelName = node->u.FILTER.rel->RelName;
	return ret;
}
vector<RID> Query_Executor::execute_tree_node_under_filter(Logical_TreeNode* node, vector<Condition*> cond)
{
	if (node->kind == Logical_TreeNode_Kind::PLAN_FILESCAN) {
		return execute_tree_node_scan(node->u.FILTER.rel, cond);
	}
	else if (node->kind == Logical_TreeNode_Kind::PLAN_JOIN) {
		return execute_tree_node_join(node->u.FILTER.rel, cond);
	}
}
vector<RID> Query_Executor::execute_tree_node_join(Logical_TreeNode* node, vector<Condition*> cond)
{
	vector<RID> ret;
	vector<Attr_Info> attrs;

	vector<RID> LRID = execute_tree_node(node->u.JOIN.left);
	vector<RID> RRID = execute_tree_node(node->u.JOIN.right);
	node->RelName = node->u.JOIN.left->RelName + "&" + node->u.JOIN.right->RelName;
	/*
	新建数据表，字段为a.x,b.y，表名为a&b
	*/
	attrs = Subsystem1_Manager::BASE.lookup_Attrs(node->RelName);

	vector<Attr_Info> Lattr = Subsystem1_Manager::BASE.lookup_Attrs(node->u.JOIN.left->RelName);
	vector<Attr_Info> Rattr = Subsystem1_Manager::BASE.lookup_Attrs(node->u.JOIN.right->RelName);
	Rel_Info LRel,RRel;
	Subsystem1_Manager::BASE.lookup_Rel(node->u.JOIN.left->RelName,LRel);
	Subsystem1_Manager::BASE.lookup_Rel(node->u.JOIN.right->RelName, RRel);
	int Llength = Lattr.back().Offset + Lattr.back().Length;
	int Rlength = Rattr.back().Offset + Rattr.back().Length;
	/*
	新建记录，但不插入
	筛选 record_cmp(cond[0], record, attrs);
	插入记录
	*/
	for (int i = 0; i < LRID.size(); i++) {
		for (int j = 0; j < RRID.size(); j++) {
			char* record = new char[1000];
			char* left = Subsystem1_Manager::BASE.Find_Record_by_RID(LRID[i]);
			char* right = Subsystem1_Manager::BASE.Find_Record_by_RID(RRID[i]);
			memcpy(record, left, Llength);
			memcpy(record + Llength, right, Rlength);

			if (record_cmp(cond[0], record, attrs)) {
				RID rid=Subsystem1_Manager::BASE.Insert_Reocrd(node->RelName, record);
				ret.push_back(rid);
			}

		}
	}

	return ret;
}
vector<RID> Query_Executor::execute_tree_node_proj(Logical_TreeNode* node)
{
	vector<RID> ret;
	vector<RID> Records = execute_tree_node(node->u.PROJECTION.rel);
	node->RelName = node->u.PROJECTION.rel->RelName + "proj";
	/*
	根据投影字段信息新建数据表并进行投影，将ret设为新表的RID
	*/
	Attr_Info* Proj_Attrs = node->u.PROJECTION.Attr_list;
	int Proj_Attrs_Num = node->u.PROJECTION.Attr_Num;
	/*构建新数据表*/
	vector<Attr_Info> attrs = Subsystem1_Manager::BASE.lookup_Attrs(node->RelName);
	for (int i = 0; i < Records.size(); i++) {
		char* record = Subsystem1_Manager::BASE.Find_Record_by_RID(Records[i]);
		char* new_record = new char[100];
		for (int j = 0; j < Proj_Attrs_Num; j++) {
			int offset = Proj_Attrs[j].Offset;
			int length = Proj_Attrs[j].Length;
			memcpy(new_record + attrs[j].Offset, record + offset, length);
		}
		RID rid = Subsystem1_Manager::BASE.Insert_Reocrd(node->RelName, new_record);
		ret.push_back(rid);
	}
	return ret;
}
bool Query_Executor::record_cmp(Condition* cond, char* record, vector<Attr_Info> attrs) {
	/*
	* 待完善 
	*/
	return true;
}
bool Query_Executor::record_cmp(Condition* cond, RID record, vector<Attr_Info> attrs) {
	/*
	* 待完善
	*/
	return true;
}
