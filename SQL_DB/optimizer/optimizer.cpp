#include "optimizer.h"
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

int Optimizer::estimate_order_lost(vector<int> Order)
{
    return 0;
}

int Optimizer::estimate_link_lost(Execution_Plan Plan1, Execution_Plan Plan2, vector<Condition> Cond)
{
    cout << "xxxx";
    cout << "yyyy";

    return 0;
}

int Optimizer::Scan_Lost(Execution_Plan Plan)
{
    return 0;
}

int Optimizer::estimate_scan_lost(Execution_Plan Plan)
{
    int Page_Capacity = 0;
    int All_Record_Num = 0;
    /*索引查找*/
    if (Plan.Index.size() != 0) {
        int num = estimate_record_num(Plan); //索引中可以查找到多少项
        int Page_Capacity_Index = 0;     //一个块中可以放几个index
        int Index_Page_Num = num / Page_Capacity_Index;  //需要访问几个index页
        int Page_Num = All_Record_Num / Page_Capacity; //记录总共存放在多少个块中
        return num / All_Record_Num * Page_Num + Index_Page_Num;  
    }
    /*扫描查找*/
    else {
        int Page_Num = All_Record_Num / Page_Capacity;
        return Page_Num;
    }
}

int Optimizer::estimate_record_num(Execution_Plan Plan)
{
    return 0;
}

bool Optimizer::IsBinary(Condition cond)
{
    return cond.bRhsIsAttr;
}

vector<Execution_Plan> Optimizer::get_Link_Order()
{
    return vector<Execution_Plan>();
}
