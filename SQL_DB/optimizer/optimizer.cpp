#include "optimizer.h"
#include "../Subsystem1.h"
#include "../Subsystem1.h"

bool Optimizer::lookup_Index(string RelName, string AttrName, Index_Info& index)
{
    return Subsystem1_Manager::BASE.lookup_Index(RelName, AttrName, index);
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

void Optimizer::init()
{
    //区分一元和二元条件
    vector<Condition> cond_temp;
    for (int cond_cnt = 0; cond_cnt < Conds.size(); ++cond_cnt)
        if (Conds[cond_cnt].bRhsIsAttr) Binary_Cond.push_back(Conds[cond_cnt]);
        else cond_temp.push_back(Conds[cond_cnt]);
    //装填执行计划
    for (int plan_cnt = 0; plan_cnt < Rels.size(); ++plan_cnt) {
        Execution_Plan plan;
        plan.Rel = Rels[plan_cnt];
        plan.type = SIMPLE;//先不管外层内层的事
        vector<Condition>::iterator it = cond_temp.begin();
        while (it != cond_temp.end()) {
            if (!strcmp(plan.Rel.Rel_Name, it->lhsAttr.relname)) {
                plan.Conds.push_back(*it);
                vector<Condition>::iterator it_temp = it;
                it++;
                cond_temp.erase(it_temp);
                Index_Info index_info;
                if (lookup_Index(plan.Rel.Rel_Name, it->lhsAttr.attrname, index_info))
                    plan.Index.push_back(index_info);
            }
            else it++;
        }

    }
}

Optimizer::Optimizer(int Rel_num, RelInfo* rels, int Attr_num, AggRelAttr* attrs,
    int Cond_num, Condition* conds, SQL_type sql_type)
{
    /*
    警告

    nickname问题未处理，应该先将所有nickname换为原名
    
    */
    if(sql_type==SELECT){
        //将语法树中的结构转换为标准结构
        for (int i = 0; i < Rel_num; i++) {
            Rel_Info temp;
            Subsystem1_Manager::BASE.lookup_Rel(rels[i].relname, temp);
            Rels.push_back(temp);
        }
        for (int i = 0; i < Attr_num; i++) {
            if (strcmp(attrs[i].attrname, "*") == 0) continue;
            Attr_Info temp;
            Subsystem1_Manager::BASE.lookup_Attr(attrs[i].relname, attrs[i].attrname, temp);
            Attrs.push_back(temp);
        }
        for (int i = 0; i < Cond_num; i++) {
            this->Conds.push_back(conds[i]);
        }
    }

    //获取逻辑树
    this->Logical_Tree_Root = Logical_Tree_Builder(Rels, Attrs, Conds).get_tree_root();
    {
        //优化逻辑树

        //rewirte

        //确定连接顺序
    }
}

vector<Execution_Plan> Optimizer::get_Link_Order()
{
    return vector<Execution_Plan>();
}
