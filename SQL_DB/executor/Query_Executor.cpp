#include "Query_Executor.h"
#include "../Subsystem1.h"
#include "../parser/parser_token.h"
Query_Executor::Query_Executor(Logical_TreeNode* Root)
{
	this->Root = Root;
}

vector<RID> Query_Executor::query()
{
	vector<void*> cond;
	return execute_tree_node(Root, cond);
}

vector<RID> Query_Executor::execute_tree_node(Logical_TreeNode* node, vector<void*>& cond, vector<Attr_Info>& attrs)
{
	vector<RID> ret;
	if (node->kind == Logical_TreeNode_Kind::PLAN_FILESCAN) {
		attrs = Subsystem1_Manager::BASE.lookup_Attrs(node->u.FILESCAN.Rel);
		string key;
		bool hasIndex = false;
		Index_Info index;
		Condition* key_cond = NULL;
		vector<void*>::iterator it = cond.begin();
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
			vector<Attr_Info> attrs = Subsystem1_Manager::BASE.lookup_Attrs(node->u.FILESCAN.Rel);
			while ((record = reader->get_Next_Record_with_RID(temp)) != NULL) {
				for (int i = 0; i < cond.size(); ++i) 
					if (!record_cmp((Condition*)cond[i],record, attrs)) continue;
				ret.push_back(temp);
			}
			return ret;
		}
		Reader* reader = new Scan_Reader(node->u.FILESCAN.Rel);
		RID temp;
		char* record = NULL;
		vector<Attr_Info> attrs = Subsystem1_Manager::BASE.lookup_Attrs(node->u.FILESCAN.Rel);
		while ((record = reader->get_Next_Record_with_RID(temp)) != NULL) {
			for (int i = 0; i < cond.size(); ++i)
				if (!record_cmp((Condition*)cond[i], record, attrs)) continue;
			ret.push_back(temp);
		}
		cond.clear();
		return ret;
	}

	else if (node->kind == Logical_TreeNode_Kind::PLAN_FILTER) {
		
		vector<RID> Records;
		if (!((Condition*)node->u.FILTER.expr_filter)->bRhsIsAttr)
			cond.push_back(node->u.FILTER.expr_filter);//一元条件落到叶结点执行
		Records = execute_tree_node(node->u.FILTER.rel, cond);
		//二元条件的筛选
		if (((Condition*)node->u.FILTER.expr_filter)->bRhsIsAttr) {
			vector<RID>::iterator it = Records.begin();
			while (it != Records.end()) {
				if (record_cmp(((Condition*)node->u.FILTER.expr_filter), *it, attrs)) {
					it++;
					continue;
				}
				vector<RID>::iterator it_next = it + 1;
				Records.erase(it);
				it = it_next;
			}
		}
		return ret;
	}

	else if (node->kind == Logical_TreeNode_Kind::PLAN_JOIN) {
		vector<RID> LRecords = execute_tree_node(node->u.JOIN.left, cond, attrs);
		vector<Attr_Info> temp = attrs;
		vector<RID> RRecords = execute_tree_node(node->u.JOIN.right, cond, attrs);
		temp.insert(temp.end(), attrs.begin(), attrs.end());
		attrs = temp;
		//内容拼接
		
		//创建新表
		//笛卡尔积
		

		return ret;
	}

	else if (node->kind == Logical_TreeNode_Kind::PLAN_PROJ) {
		vector<RID> Records = execute_tree_node(node->u.PROJECTION.rel, cond);
		/*
		根据投影字段信息新建数据表并进行投影，将ret设为新表的RID
		*/
	}

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
