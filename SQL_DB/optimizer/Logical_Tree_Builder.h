#pragma once

#include <iostream>
#include "../parser/parser_interp.h"
#include "../global.h"
using namespace std;


enum Logical_TreeNode_Kind {
    /*stmt node tags*/
    PLAN_SELECT,
    PLAN_INSERT,
    PLAN_DELETE,
    PLAN_UPDATE,
    PLAN_REPLACE,
    /*op node tags*/
    PLAN_FILESCAN, /* Relation     ��ϵ��Ҷ�ӽڵ� */
    PLAN_SCAN,
    PLAN_FILTER,   /* Selection  ѡ��   */
    PLAN_PROJ,     /* Projection ͶӰ*/
    PLAN_JOIN,     /* Join       ���� ��ָ��ֵ����*/
    PLAN_DIST,     /* Duplicate elimination( Distinct) ��������*/
    PLAN_GROUP,    /* Grouping   ����(�����˾ۼ�)*/
    PLAN_SORT,     /* Sorting    ����*/
    PLAN_LIMIT,
    /*support node tags*/
    PLAN_LIST
};

struct Logical_TreeNode {

    Logical_TreeNode_Kind kind;
    string RelName;
    union U{
        struct FILTER {
            struct Logical_TreeNode* rel;  //���ݱ�
            Condition* expr_filter; //����,ǿ��ת��ΪCondition��ʹ��
        }FILTER;
        struct PROJECTION {
            struct Logical_TreeNode* rel;  //���ݱ�
            int Attr_Num;
            Attr_Info* Attr_list;  //ͶӰ�ֶ�,Attr_Info*
        }PROJECTION;
        struct JOIN {
            struct Logical_TreeNode* left;
            struct Logical_TreeNode* right;
        }JOIN;
        struct FILESCAN {
            char Rel[100];  //���ݱ�  
        }FILESCAN;   //Ҷ�ڵ�
    }u;
};
static Logical_TreeNode All_Logical_Tree_Node[100];
static int All_Logical_Tree_Node_Ptr = 0;
static Logical_TreeNode* get_logical_tree_node(Logical_TreeNode_Kind kind) {
    Logical_TreeNode* ret = All_Logical_Tree_Node + All_Logical_Tree_Node_Ptr;
    All_Logical_Tree_Node_Ptr++;
    ret->kind = kind;
    return ret;
}

/*����ʹ�÷�ʽ����һ���ǹ��캯���������ԡ��ڶ�����get_tree_node�Ŵ���*/
class Logical_Tree_Builder {
	//���﷨��ת��Ϊ�߼���
private:
    Logical_TreeNode* Root;
    vector<Rel_Info> Rels;
    vector<Attr_Info> Attrs;
    vector<Condition> Conds;

    //���߲�ͬ�Ķ�Ԫ����
    map<vector<string>, Logical_TreeNode*> relation_to_binary_condition_node_map;
    //Ҷ�ӽ�㡢һԪ������������ͬ�Ķ�Ԫ����
    map<string, Logical_TreeNode*> relation_to_node_map;

public:
    Logical_Tree_Builder(vector<Rel_Info> Rels, vector<Attr_Info> Attrs, vector<Condition> Conds);

    //GA��
    Logical_Tree_Builder(map<vector<string>, Logical_TreeNode*> relation_to_binary_condition_node_map,
        map<string, Logical_TreeNode*> relation_to_node_map) : Root (NULL), relation_to_node_map(relation_to_node_map),
        relation_to_binary_condition_node_map(relation_to_binary_condition_node_map){}
    
    
    Logical_TreeNode* get_tree_root();
    //����Ƿ���ڶ�Ԫ��������������򷵻��µĽ�㣬����������򷵻�ԭ����node
    Logical_TreeNode* add_node_to_binary_condition_node(Logical_TreeNode* node, string rel1, string rel2);
    //����ĳ������˳��ȷ���߼�����order�ڲ�ΪRelName
    //GA��
    Logical_TreeNode* get_tree_root_with_order(vector<string> order);
    static void display(Logical_TreeNode *Root);

    static void delete_node(Logical_TreeNode* root);
};