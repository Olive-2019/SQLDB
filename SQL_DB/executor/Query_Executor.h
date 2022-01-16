#pragma once
#include "../optimizer/optimizer.h"
#include "../RECORD.h"
#include "../optimizer/Logical_Tree_Builder.h"
class Query_Executor {

private:
	Logical_TreeNode* Root;
	//��¼�������ĶԱ�
	bool record_cmp(Condition* cond, char* record, string RelName);
	bool record_cmp(Condition* cond, RID rid, string RelName);
	//һԪ����cmp
	bool record_cmp_1(Condition* cond, char* record, string RelName);
	//��Ԫ����cmp
	bool record_cmp_2(Condition* cond, char* record, string RelName);
	//��Ԫ�����ȽϺ�����0��ȣ�1���ڣ�-1С��
	int cmp(char* p1, char* p2, Attr_Info Lattr, Attr_Info Rattr);
	//һԪ�����ȽϺ���
	int cmp(char* p1, Value p2, Attr_Info Lattr);
	float get_value(char* value, AttrType type);
public:
	Query_Executor(Logical_TreeNode* Root);
	//ִ�в�ѯ
	vector<RID> query();
	//�Ծ�������е�execute
	vector<RID> execute_tree_node(Logical_TreeNode* node);
	//��Ҷ�ڵ��ִ�У�condsΪһԪ����
	vector<RID> execute_tree_node_scan(Logical_TreeNode* node, vector<Condition*> cond = vector<Condition*>());
	//�������е�filter���õ�
	vector<RID> execute_tree_node_filter(Logical_TreeNode* node);
	//һԪ��filter��һԪ�����³�
	vector<RID> execute_tree_node_under_filter(Logical_TreeNode* node, vector<Condition*>& cond);
	//join���Զ�Ԫ�����³�
	vector<RID> execute_tree_node_join(Logical_TreeNode* node, vector<Condition*> cond = vector<Condition*>());
	//ͶӰ���Ĵ���
	vector<RID> execute_tree_node_proj(Logical_TreeNode* node);
};