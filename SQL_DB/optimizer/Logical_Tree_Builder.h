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
};

struct Logical_TreeNode {

    Logical_TreeNode_Kind kind;
    string RelName;
    union U{
        struct FILTER {
            struct Logical_TreeNode* rel;  //数据表
            Condition* expr_filter; //条件,强制转换为Condition后使用
        }FILTER;
        struct PROJECTION {
            struct Logical_TreeNode* rel;  //数据表
            int Attr_Num;
            Attr_Info* Attr_list;  //投影字段,Attr_Info*
        }PROJECTION;
        struct JOIN {
            struct Logical_TreeNode* left;
            struct Logical_TreeNode* right;
        }JOIN;
        struct FILESCAN {
            char Rel[100];  //数据表  
        }FILESCAN;   //叶节点
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

/*两种使用方式，第一种是构造函数传入属性、第二种是get_tree_node才传入*/
class Logical_Tree_Builder {
	//将语法树转变为逻辑树
private:
    Logical_TreeNode* Root;
    vector<Rel_Info> Rels;
    vector<Attr_Info> Attrs;
    vector<Condition> Conds;

    //两边不同的二元条件
    map<vector<string>, Logical_TreeNode*> relation_to_binary_condition_node_map;
    //叶子结点、一元条件、两边相同的二元条件
    map<string, Logical_TreeNode*> relation_to_node_map;

public:
    Logical_Tree_Builder(vector<Rel_Info> Rels, vector<Attr_Info> Attrs, vector<Condition> Conds);

    //GA用
    Logical_Tree_Builder(map<vector<string>, Logical_TreeNode*> relation_to_binary_condition_node_map,
        map<string, Logical_TreeNode*> relation_to_node_map) : Root (NULL), relation_to_node_map(relation_to_node_map),
        relation_to_binary_condition_node_map(relation_to_binary_condition_node_map){}
    
    
    Logical_TreeNode* get_tree_root();
    //检查是否存在二元条件，如果存在则返回新的结点，如果不存在则返回原来的node
    Logical_TreeNode* add_node_to_binary_condition_node(Logical_TreeNode* node, string rel1, string rel2);
    //根据某种连接顺序确定逻辑树，order内部为RelName
    //GA用
    Logical_TreeNode* get_tree_root_with_order(vector<string> order);
    static void display(Logical_TreeNode *Root);

    static void delete_node(Logical_TreeNode* root);
};