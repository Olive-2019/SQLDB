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



bool Optimizer::IsBinary(Condition cond)
{
    return cond.bRhsIsAttr;
}

vector<Execution_Plan> Optimizer::get_Link_Order()
{
    return vector<Execution_Plan>();
}
