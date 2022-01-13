#include "Query_Executor.h"
#include "../Subsystem1.h"
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
	if (node->kind == Logical_TreeNode_Kind::PLAN_FILESCAN) {
		Reader* reader = new Scan_Reader(node->u.FILESCAN.Rel);
		RID temp;
		while (reader->get_Next_Record_with_RID(temp) != NULL) {
			ret.push_back(temp);
		}
		return ret;
	}

	else if (node->kind == Logical_TreeNode_Kind::PLAN_FILTER) {
		vector<RID> Records = execute_tree_node(node->u.FILTER.rel);
		/*
		���ɸѡ
		*/
		return ret;
	}

	else if (node->kind == Logical_TreeNode_Kind::PLAN_JOIN) {
		vector<RID> LRecords = execute_tree_node(node->u.JOIN.left);
		vector<RID> RRecords = execute_tree_node(node->u.JOIN.right);
		/*
		����һ�������ݱ����棬��ret��Ϊ�±��RID
		*/
		return ret;
	}

	else if (node->kind == Logical_TreeNode_Kind::PLAN_PROJ) {
		vector<RID> Records = execute_tree_node(node->u.PROJECTION.rel);
		/*
		����ͶӰ�ֶ���Ϣ�½����ݱ�����ͶӰ����ret��Ϊ�±��RID
		*/
	}

}
