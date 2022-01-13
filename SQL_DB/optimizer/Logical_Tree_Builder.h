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
    union U{

        struct FILTER {
            struct Logical_TreeNode* rel;  //���ݱ�
            void* expr_filter; //����,ǿ��ת��ΪCondition��ʹ��
        }FILTER;
        struct PROJECTION {
            struct Logical_TreeNode* rel;  //���ݱ�
            int Attr_Num;
            void* Attr_list;  //ͶӰ�ֶ�,Attr_Info*
        }PROJECTION;
        struct JOIN {
            struct Logical_TreeNode* left;
            struct Logical_TreeNode* right;
        }JOIN;
        struct FILESCAN {
            char* Rel;  //���ݱ�  
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


class Logical_Tree_Builder {
	//���﷨��ת��Ϊ�߼���
private:
    Logical_TreeNode* Root;
    vector<Rel_Info> Rels;
    vector<Attr_Info> Attrs;
    vector<Condition> Conds;

public:
    Logical_Tree_Builder(vector<Rel_Info> Rels, vector<Attr_Info> Attrs, vector<Condition> Conds);
    
    
    Logical_TreeNode* get_tree_root();
    //����ĳ������˳��ȷ���߼�����order�ڲ�ΪRelName
    Logical_TreeNode* get_tree_root_order(vector<string> order);
    void display();
};