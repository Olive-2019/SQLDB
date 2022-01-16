#include "Query_Executor.h"
#include "../Subsystem1.h"
#include "../parser/parser_token.h"

/*
警告
由于所有的attrname与relname都指向同一处内存
同时在进行join处理时需要修改cond中的attrname与relname
可能导致问题
但此处为最后处理execute，后续不会再调用该内存
因此可能没问题
*/


Query_Executor::Query_Executor(Logical_TreeNode* Root)
{
	this->Root = Root;
}

string Query_Executor::get_final_RelName()
{
	return Root->RelName;
}

vector<RID> Query_Executor::query()
{
	return execute_tree_node(Root);
}

vector<RID> Query_Executor::execute_tree_node(Logical_TreeNode* node)
{
	/*
	警告
	在进行join后，由于新表中属性名变化导致filter节点中cond属性名需要调整
	已修改 execute tree node join中
	*/
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
		//针对当前索引获取reader
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
		while ((record = reader->get_Next_Record_with_RID(temp)) != NULL) {
			for (int i = 0; i < cond.size(); ++i)
				if (!record_cmp(cond[i], record, node->RelName)) continue;
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
			if (!record_cmp(cond[i], record, node->RelName)) continue;
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
vector<RID> Query_Executor::execute_tree_node_under_filter(Logical_TreeNode* node, vector<Condition*>& cond)
{
	cond.push_back(node->u.FILTER.expr_filter);
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
	int Llength = Lattr.back().Offset + Lattr.back().Length;
	int Rlength = Rattr.back().Offset + Rattr.back().Length;
	//新表的属性
	vector<Attr_Info> new_attr = Lattr;
	//对新表属性名的修正：加入表名作为前缀
	if (node->u.JOIN.left->RelName.find('&') == string::npos) {
		for (int i = 0; i < Lattr.size(); ++i) {
			char buff[100] = {0};
			strcpy(buff, node->u.JOIN.left->RelName.c_str());
			strcat(buff, ".");
			strcat(buff, new_attr[i].Attr_Name);
			strcpy(new_attr[i].Attr_Name, buff);
		}
	}
	new_attr.insert(new_attr.end(), Rattr.begin(), Rattr.end());
	if (node->u.JOIN.right->RelName.find('&') == string::npos) {
		for (int i = 0; i < Rattr.size(); ++i) {
			char buff[100] = { 0 };
			strcpy(buff, node->u.JOIN.right->RelName.c_str());
			strcat(buff, ".");
			strcat(buff, new_attr[Lattr.size()+i].Attr_Name);
			strcpy(new_attr[Lattr.size() + i].Attr_Name, buff);
		}
	}
	//对新表属性offset的修正：第二张表的offset要加上第一张表的length
	for (int right_index = Lattr.size(); right_index < Lattr.size() + Rattr.size(); ++right_index) 
		new_attr[right_index].Offset += Llength;
	//对新表属性所属表名的修正
	for (int new_attr_index = 0; new_attr_index < new_attr.size(); ++new_attr_index)
		strcpy(new_attr[new_attr_index].Rel_Name, node->RelName.c_str());
	//创建新表
	Subsystem1_Manager::BASE.Create_Rel(node->RelName, new_attr);
	Rel_Info LRel,RRel;
	Subsystem1_Manager::BASE.lookup_Rel(node->u.JOIN.left->RelName,LRel);
	Subsystem1_Manager::BASE.lookup_Rel(node->u.JOIN.right->RelName, RRel);
	
	/*
	修改cond中的属性名
	*/

	{
		for (int i = 0; i < cond.size(); i++) {
			string attrName = string(cond[i]->lhsAttr.relname) + "." + cond[i]->lhsAttr.attrname;
			strcpy(cond[i]->lhsAttr.relname, node->RelName.c_str());
			strcpy(cond[i]->lhsAttr.attrname, attrName.c_str());
			if (cond[i]->bRhsIsAttr) {
				string attrName = string(cond[i]->rhsAttr.relname) + "." + cond[i]->rhsAttr.attrname;
				strcpy(cond[i]->rhsAttr.relname, node->RelName.c_str());
				strcpy(cond[i]->rhsAttr.attrname, attrName.c_str());
			}
		}
	}




	/*
	新建记录，但不插入（注释忘改了？这里貌似是插入了的）
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
			//不一定只有一个条件的，这里改动了一下
			bool pass = true;
			for (int cond_index = 0; cond_index < cond.size(); ++cond_index) {
				pass = record_cmp(cond[cond_index], record, node->RelName);
				if (!pass) break;
			}
			//没通过检查的话，这一条记录就不插入了。
			if (!pass) continue;
			RID rid=Subsystem1_Manager::BASE.Insert_Reocrd(node->RelName, record);
			ret.push_back(rid);
		}
	}
	if (node->u.JOIN.left->RelName.find('&') != string::npos) Subsystem1_Manager::BASE.Delete_Rel(node->u.JOIN.left->RelName);
	if (node->u.JOIN.right->RelName.find('&') != string::npos) Subsystem1_Manager::BASE.Delete_Rel(node->u.JOIN.right->RelName);
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
	//对未join过的表的特殊处理
	if (node->RelName.find('&') == string::npos) {
		for (int i = 0; i < attrs.size(); ++i) {
			char buff[50];
			strcpy(buff, attrs[i].Rel_Name);
			strcat(buff, ".");
			strcat(buff, attrs[i].Attr_Name);
			strcat(attrs[i].Attr_Name, buff);
		}
	}
	vector<Attr_Info> new_attrs;
	
	int offset = 0;
	//记录的是从投影的属性到子节点属性的对应下标
	vector<int> proj_attr_to_attrs_index;
	//将投影后的新属性和映射表整理出来
	for (int proj_attr_index = 0; proj_attr_index < Proj_Attrs_Num; ++proj_attr_index) {
		Attr_Info tmp = Proj_Attrs[proj_attr_index];
		strcpy(tmp.Attr_Name, Proj_Attrs[proj_attr_index].Rel_Name);
		strcat(tmp.Attr_Name, ".");
		strcat(tmp.Attr_Name, Proj_Attrs[proj_attr_index].Attr_Name);
		tmp.Offset = offset;
		offset += tmp.Length;
		strcpy(tmp.Rel_Name, node->RelName.c_str());
		bool error = true;
		for (int attr_index = 0; attr_index < attrs.size(); ++attr_index) 
			if (!strcmp(tmp.Attr_Name, attrs[attr_index].Attr_Name)) {
				proj_attr_to_attrs_index.push_back(attr_index);
				error = false;
				break;
			}
		if (error) cout << "投影属性出错" << endl;
	}
	for (int i = 0; i < Records.size(); i++) {
		char* record = Subsystem1_Manager::BASE.Find_Record_by_RID(Records[i]);
		char* new_record = new char[500];
		for (int j = 0; j < Proj_Attrs_Num; j++) {
			memcpy(new_record + new_attrs[j].Offset, 
				record + attrs[proj_attr_to_attrs_index[j]].Offset, new_attrs[j].Length);
		}
		RID rid = Subsystem1_Manager::BASE.Insert_Reocrd(node->RelName, new_record);
		ret.push_back(rid);
	}
	return ret;
}



bool Query_Executor::record_cmp(Condition* cond, char* record, string RelName) {
	if (cond->bRhsIsAttr) {
		return record_cmp_2(cond, record, RelName);
	}
	else {
		return record_cmp_1(cond, record, RelName);
	}
}
bool Query_Executor::record_cmp_1(Condition* cond, char* record, string RelName)
{
	Attr_Info Lattr;
	Subsystem1_Manager::BASE.lookup_Attr(RelName, cond->lhsAttr.attrname, Lattr);
	char* Lvalue = record + Lattr.Offset;
	int ret = cmp(Lvalue, cond->rhsValue, Lattr);
	switch (cond->op) {
	case TOKENKIND::T_EQ: {
		return ret == 0;
		break;
	}
	case TOKENKIND::T_NE: {
		return ret != 0;
		break;
	}
		
	case TOKENKIND::T_GE: {
		return ret >= 0;
		break;
	}
		
	case TOKENKIND::T_GT: {
		return ret > 0;
		break;
	}
		
	case TOKENKIND::T_LE: {
		return ret <= 0;
		break;
	}
		
	case TOKENKIND::T_LT: {
		return ret < 0;
		break;
	}
	}
}
bool Query_Executor::record_cmp_2(Condition* cond, char* record, string RelName)
{
	Attr_Info Lattr, Rattr;
	Subsystem1_Manager::BASE.lookup_Attr(RelName, cond->lhsAttr.attrname, Lattr);
	Subsystem1_Manager::BASE.lookup_Attr(RelName, cond->rhsAttr.attrname, Rattr);
	char* Lvalue = record + Lattr.Offset;
	char* Rvalue = record + Rattr.Offset;
	int ret = cmp(Lvalue, Rvalue, Lattr, Rattr);
	switch (cond->op) {
	case TOKENKIND::T_EQ: {
		return ret == 0;
		break;
	}
	case TOKENKIND::T_NE: {
		return ret != 0;
		break;
	}

	case TOKENKIND::T_GE: {
		return ret >= 0;
		break;
	}

	case TOKENKIND::T_GT: {
		return ret > 0;
		break;
	}

	case TOKENKIND::T_LE: {
		return ret <= 0;
		break;
	}

	case TOKENKIND::T_LT: {
		return ret < 0;
		break;
	}
	}
}

int Query_Executor::cmp(char* p1, Value p2, Attr_Info Lattr)
{

	if (Lattr.type == INT || Lattr.type == FLOAT) {
		float Lvalue = get_value(p1, Lattr.type);
		float Rvalue = get_value((char*)p2.data, p2.type);
		if (Lvalue == Rvalue) return 0;
		if (Lvalue < Rvalue) return -1;
		if (Lvalue > Rvalue) return 1;
	}
	else {
		int Rlength = strlen((char*)p2.data);
		int Llength = Lattr.Length;
		int length = min(Llength, Rlength);
		int ret = strncmp(p1, (char*)p2.data, length);
		if (ret == 0) {
			if (Lattr.Length == Rlength) ret = 0;
			if (Lattr.Length > Rlength) ret = 1;
			if (Lattr.Length < Rlength) ret = -1;
		}
		return ret;
	}
	return 0;
}

int Query_Executor::cmp(char* p1, char* p2, Attr_Info Lattr, Attr_Info Rattr)
{
	/*
	警告
	是否允许不同类型之间的比较 如int float之间以及string10 string20之间
	当前代码允许
	*/


	//INT与FLOAT INT与INT FLOAT与FLOAT FLOAT与INT之间比较
	if ((Lattr.type == INT|| Lattr.type == FLOAT) && Rattr.type != STRING) {
		float Lvalue = get_value(p1, Lattr.type);
		float Rvalue = get_value(p2, Rattr.type);
		if (Lvalue == Rvalue) return 0;
		if (Lvalue < Rvalue) return -1;
		if (Lvalue > Rvalue) return 1;
	}
	//STRING之间比较
	else {
		int length = min(Lattr.Length, Rattr.Length);
		int ret = strncmp(p1, p2, length);
		if (ret == 0) {
			if (Lattr.Length == Rattr.Length) ret = 0;
			if (Lattr.Length > Rattr.Length) ret = 1;
			if (Lattr.Length < Rattr.Length) ret = -1;
		}
		return ret;
	}
}

float Query_Executor::get_value(char* value, AttrType type)
{
	float ret = 0;
	if (type == AttrType::INT) {
		int t = *(int*)value;
		ret = t;
	}
	else if (type == AttrType::FLOAT) {
		ret = *(float*)value;
	}
	else {
		cout << "非int、float进行值获取操作" << endl;
		exit(0);
	}
	return ret;
}

bool Query_Executor::record_cmp(Condition* cond, RID rid, string RelName) {
	char* record = Subsystem1_Manager::BASE.Find_Record_by_RID(rid);
	return record_cmp(cond, record, RelName);
}
