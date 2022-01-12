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
        U() {

        }
        struct {
            struct Logical_TreeNode* rel;  //���ݱ�
            Condition& expr_filter; //����
        }FILTER;
        struct {
            struct Logical_TreeNode* rel;  //���ݱ�
            vector<Attr_Info>& Attr_list;  //ͶӰ�ֶ�
        }PROJECTION;
        struct {
            struct Logical_TreeNode* left;
            struct Logical_TreeNode* right;
        }JOIN;
        struct {
            Rel_Info& Rel;  //���ݱ�  
        }FILESCAN;   //Ҷ�ڵ�
    }u;
};

class Logical_Tree_Builder {
	//���﷨��ת��Ϊ�߼���
private:
    Logical_TreeNode* Root;
public:
    Logical_Tree_Builder(vector<Rel_Info> Rels, vector<Attr_Info> Attrs, vector<Condition> Conds);
    Logical_TreeNode* get_tree_root() {
        return Root;
    }
};