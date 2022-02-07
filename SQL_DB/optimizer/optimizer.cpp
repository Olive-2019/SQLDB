#include "optimizer.h"
#include "../Subsystem1.h"
#include "../Subsystem1.h"
#include "DP_Link_Order_Affirmant.h"
#include "GA_Link_Order_Affirmant.h"

bool Optimizer::lookup_Index(string RelName, string AttrName, Index_Info& index)
{
    return Subsystem1_Manager::mgr.lookup_Index(RelName, AttrName, index);
}

void Optimizer::generate_execution_plan()
{
    for (int i = 0; i < Rels.size(); i++) {
        Execution_Plan Plan;
        string RelName = Rels[i].Rel_Name;
        Plan.Rel = Rels[i];
        /*检查在该表上的一元条件*/
        for (int j = 0; j < Conds.size(); j++) {
            if (IsBinary(Conds[i])) continue;
            if (RelName != Conds[i].lhsAttr.relname) continue;
            Plan.Conds.push_back(Conds[i]);
            Index_Info Index;
            bool Index_Exist = lookup_Index(RelName, Conds[i].lhsAttr.attrname, Index);
            if (Index_Exist) {
                Plan.Index.push_back(Index);
            }
        }
    }
}

void Optimizer::generate_link_order()
{
}



bool Optimizer::IsBinary(Condition cond)
{
    return cond.bRhsIsAttr;
}

void Optimizer::init(int Rel_num, RelInfo* rels, int Attr_num, AggRelAttr* attrs,
    int Cond_num, Condition* conds)
{
    {
        //将语法树中的结构转换为标准结构
        for (int i = 0; i < Rel_num; i++) {
            Rel_Info temp;
            Subsystem1_Manager::mgr.lookup_Rel(rels[i].relname, temp);
            Rels.push_back(temp);
        }
        for (int i = 0; i < Attr_num; i++) {
            if (strcmp(attrs[i].attrname, "*" ) == 0 || !Attr_num) {
                vector<Attr_Info> temp;
                for (int j = 0; j < Rel_num; j++) {
                    temp=Subsystem1_Manager::mgr.lookup_Attrs(Rels[j].Rel_Name);
                    for (int k = 0; k < temp.size(); k++) {
                        Attrs.push_back(temp[k]);
                    }
                }
                continue;
            }
            Attr_Info temp;
            Subsystem1_Manager::mgr.lookup_Attr(attrs[i].relname, attrs[i].attrname, temp);
            Attrs.push_back(temp);
        }
        for (int i = 0; i < Cond_num; i++) {
            this->Conds.push_back(conds[i]);
        }
    }

    //获取逻辑树
    //for (int i = 0; i < Rels.size(); ++i) printf("%p\n",Rels[i].Rel_Name);
    //DP_Link_Order_Affirmant link_order_affirmant = DP_Link_Order_Affirmant(Rels, Conds);
    
    //DP_Link_Order_Affirmant link_order_affirmant = DP_Link_Order_Affirmant(Rels, Conds);
    //this->Logical_Tree_Root = link_order_affirmant.get_tree();
    cout << "get tree" << endl;
    this->Logical_Tree_Root = Logical_Tree_Builder(Rels, Attrs, Conds).get_tree_root();
    {
        //优化逻辑树

        //rewirte

        //确定连接顺序
    }
}

Optimizer::Optimizer(int Rel_num, RelInfo* rels, int Attr_num, AggRelAttr* attrs,
    int Cond_num, Condition* conds, SQL_type sql_type)
{
    /*
    警告

    nickname问题未处理，应该先将所有nickname换为原名
    
    */

    
    init(Rel_num, rels, Attr_num, attrs, Cond_num, conds);
    Executor* executor = new Executor(Logical_Tree_Root);
    executor->execute_select();

}

//update语句构造函数,char**values为新值
Optimizer::Optimizer(RelInfo rel, int Attr_num, NODE* attrs, char** values, int Cond_num, Condition* conds, SQL_type sql_type)
{
    init(1, &rel, 0, nullptr, Cond_num, conds);
    Executor* executor = new Executor(Logical_Tree_Root);
    vector<Attr_Info> Attrs;
    for (int i = 0; i < Attr_num; i++) {
        Attr_Info attr;
        Subsystem1_Manager::mgr.lookup_Attr(attrs[i].u.RELATTR.relname, attrs[i].u.RELATTR.attrname, attr);
        Attrs.push_back(attr);
    }
    executor->execute_update(Attrs, values);
}
//insert语句构造函数
Optimizer::Optimizer(RelInfo rel, char* record, SQL_type sql_type)
{

    Executor* executor = new Executor(nullptr);
    executor->execute_insert(rel.relname, record);
}
//delete语句构造函数
Optimizer::Optimizer(RelInfo rel, int Cond_num, Condition* conds, SQL_type sql_type)
{
    init(1, &rel, 0, nullptr, Cond_num, conds);
    Executor* executor = new Executor(Logical_Tree_Root);
    executor->execute_delete();
}

vector<Execution_Plan> Optimizer::get_Link_Order()
{
    return vector<Execution_Plan>();
}
