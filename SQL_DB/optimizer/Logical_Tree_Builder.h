#pragma once

#include <iostream>
#include "../parser/parser_interp.h"
#include "../global.h"
using namespace std;

enum Logical_TreeNode_Kind {

};

typedef enum Logical_Tree_NODEKIND {
    /*stmt node tags*/
    PLAN_SELECT,
    PLAN_INSERT,
    PLAN_DELETE,
    PLAN_UPDATE,
    PLAN_REPLACE,
    /*op node tags*/
    PLAN_FILESCAN, /* Relation     关系，叶子节点 */
    PLAN_SCAN,
    PLAN_FILTER,   /* Selection  选择   */
    PLAN_PROJ,     /* Projection 投影*/
    PLAN_JOIN,     /* Join       连接 ，指等值连接*/
    PLAN_DIST,     /* Duplicate elimination( Distinct) 消除反复*/
    PLAN_GROUP,    /* Grouping   分组(包括了聚集)*/
    PLAN_SORT,     /* Sorting    排序*/
    PLAN_LIMIT,
    /*support node tags*/
    PLAN_LIST
}Logical_Tree_NODEKIND;

typedef struct Logical_TreeNode {

    Logical_Tree_NODEKIND kind;

    union {
        struct {
            struct Logical_TreeNode* rel;  //数据表
            Condition* expr_filter; //条件
        }FILTER;
        struct {
            struct Logical_TreeNode* rel;  //数据表
            Attr_Info* Attr_list;  //投影字段
        }PROJECTION;
        struct {
            struct Logical_TreeNode* left;
            struct Logical_TreeNode* right;
        }JOIN;
        
        struct {
            Rel_Info* Rel;  //数据表  
        }FILESCAN;   //叶节点
    }u;
}Logical_TreeNode;




class Logical_Tree_Builder {
	//将语法树转变为逻辑树
private:
    Logical_TreeNode* Root;
public:
    Logical_Tree_Builder(vector<Rel_Info> Rels, vector<Attr_Info> Attrs, vector<Condition> Conds);
    Logical_TreeNode* get_tree_root() {
        return Root;
    }
};