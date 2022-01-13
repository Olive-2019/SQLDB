#include "estimator.h"
#include "../Subsystem1.h"
double Estimator::estimate()
{
    Link_Lost lost = estimate_TreeNode_Lost(Root);

    return lost.Disk_Scan_Lost * 0.9 + lost.Memory_Scan_Lost * 0.1;
}

Link_Lost Estimator::estimate_TreeNode_Lost(Logical_TreeNode* node)
{
    Link_Lost ret;
    if (node->kind == Logical_TreeNode_Kind::PLAN_FILESCAN) {
        Rel_Info temp;
        Subsystem1_Manager::BASE.lookup_Rel(node->u.FILESCAN.Rel, temp);
        ret.record_num = temp.Record_Num;
    }
    else if (node->kind == Logical_TreeNode_Kind::PLAN_FILTER) {
        Link_Lost temp = estimate_TreeNode_Lost(node->u.FILTER.rel);
        ret = temp;
        /*
        根据条件以及rel信息确定记录数量，但disk lost应该与表扫描一致
        注意，filter下方的结点可以是叶节点，也可以是join结点
        */

        if (node->u.FILTER.rel->kind == Logical_TreeNode_Kind::PLAN_FILESCAN) {
            //根据条件涉及字段的分布信息以及条件信息获取损失
        }
        else if (node->u.FILTER.rel->kind == Logical_TreeNode_Kind::PLAN_JOIN) {
            //根据条件涉及字段的分布信息以及条件信息获取损失
        }

    }
    else if (node->kind == Logical_TreeNode_Kind::PLAN_JOIN) {
        Link_Lost tempL = estimate_TreeNode_Lost(node->u.JOIN.left);
        Link_Lost tempR = estimate_TreeNode_Lost(node->u.JOIN.right);

        ret.Disk_Scan_Lost = tempL.Disk_Scan_Lost + tempR.Disk_Scan_Lost;
        ret.record_num = tempL.record_num * tempR.record_num;
    }

    else if (node->kind == Logical_TreeNode_Kind::PLAN_PROJ) {
        Link_Lost temp = estimate_TreeNode_Lost(node->u.PROJECTION.rel);
        ret = temp;
    }

    return ret;
}

Link_Lost Estimator::estimate_scan_lost(const Execution_Plan& Plan)
{
    int Page_Capacity = 0;
    int All_Record_Num = 0;
    Link_Lost ret;
    /*索引查找*/
    if (Plan.Index.size() != 0) {
        int num = estimate_record_num(Plan); //索引中可以查找到多少项
        int Page_Capacity_Index = 0;     //一个块中可以放几个index
        int Index_Page_Num = num / Page_Capacity_Index;  //需要访问几个index页
        int Page_Num = All_Record_Num / Page_Capacity; //记录总共存放在多少个块中
        ret.Disk_Scan_Lost = num / All_Record_Num * Page_Num + Index_Page_Num;
    }
    /*扫描查找*/
    else {
        int Page_Num = All_Record_Num / Page_Capacity;
        ret.Disk_Scan_Lost = Page_Num;
    }

    return ret;
}

int Estimator::estimate_record_num(const Execution_Plan& Plan)
{
    return 0;
}