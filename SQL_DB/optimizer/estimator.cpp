#include "estimator.h"
#include "../Subsystem1.h"
double Estimator::estimate()
{
    Link_Lost lost = estimate_TreeNode_Lost(Root);

    return lost.Disk_Scan_Lost * 0.9 + lost.Memory_Scan_Lost * 0.1;
}

Link_Lost Estimator::estimate_TreeNode_Lost_Scan(Logical_TreeNode* node)
{
    Link_Lost ret;
    Rel_Info temp;
    Subsystem1_Manager::BASE.lookup_Rel(node->u.FILESCAN.Rel, temp);

    vector<Attr_Info> attrs = Subsystem1_Manager::BASE.lookup_Attrs(temp.Rel_Name);
    int record_length = attrs.back().Offset + attrs.back().Length;
    ret.Memory_Scan_Lost = 0;
    //ret.record_num = temp.Record_Num;
    table_to_record_num[temp.Rel_Name] = temp.Record_Num;
    ret.Disk_Scan_Lost = Global_Paras::Block_Size / (ret.record_num * record_length);
    
    return ret;
}

Link_Lost Estimator::estimate_TreeNode_Lost_Filter(Logical_TreeNode* node)
{
    Link_Lost ret;
    //这个地方是不是执行重复了？下面的代码就是处理下一个结点的
    Link_Lost temp = estimate_TreeNode_Lost(node->u.FILTER.rel);
    ret = temp;
    /*
    根据条件以及rel信息确定记录数量，但disk lost应该与表扫描一致
    注意，filter下方的结点可以是叶节点，也可以是join结点和filter
    */
    if (node->u.FILTER.rel->kind == Logical_TreeNode_Kind::PLAN_FILESCAN) {
        Link_Lost lost1 = estimate_TreeNode_Lost_Scan(node->u.FILTER.rel);
        ret = lost1;
        /*
        根据条件以及分布
        减少ret的record num
        */
        
    }
    else if (node->u.FILTER.rel->kind == Logical_TreeNode_Kind::PLAN_JOIN) {
        Link_Lost lost1 = estimate_TreeNode_Lost_Join(node->u.FILTER.rel);
        ret = lost1;
    }
    else if (node->u.FILTER.rel->kind == Logical_TreeNode_Kind::PLAN_FILTER) {
        ret = estimate_TreeNode_Lost_Filter(node->u.FILTER.rel);
    }
    
    if (!node->u.FILTER.expr_filter->bRhsIsAttr) {
        ret.record_num = estimate_record_num(*node->u.FILTER.expr_filter, ret.record_num);
    }
    
    ret.Memory_Scan_Lost += ret.record_num;
    return ret;
}

Link_Lost Estimator::estimate_TreeNode_Lost_Join(Logical_TreeNode* node)
{
    Link_Lost ret;
    Link_Lost tempL = estimate_TreeNode_Lost(node->u.JOIN.left);
    Link_Lost tempR = estimate_TreeNode_Lost(node->u.JOIN.right);

    ret.Disk_Scan_Lost = tempL.Disk_Scan_Lost + tempR.Disk_Scan_Lost;
    ret.Memory_Scan_Lost= tempL.record_num * tempR.record_num;
    ret.record_num = tempL.record_num * tempR.record_num;
    return ret;
}

Link_Lost Estimator::estimate_TreeNode_Lost_Proj(Logical_TreeNode* node)
{
    Link_Lost ret = estimate_TreeNode_Lost(node->u.PROJECTION.rel);
    int record_length = 0;
    int attr_num = node->u.PROJECTION.Attr_Num;
    Attr_Info* attrs = node->u.PROJECTION.Attr_list;
    for (int i = 0; i < attr_num; i++) {
        record_length += attrs[i].Length;
    }
    ret.Disk_Scan_Lost+= Global_Paras::Block_Size / (ret.record_num * record_length);
    return ret;
}

Link_Lost Estimator::estimate_TreeNode_Lost(Logical_TreeNode* node)
{
    Link_Lost ret;
    if (node->kind == Logical_TreeNode_Kind::PLAN_FILESCAN) {
        return estimate_TreeNode_Lost_Scan(node);
    }
    else if (node->kind == Logical_TreeNode_Kind::PLAN_FILTER) {
        return estimate_TreeNode_Lost_Filter(node);
    }
    else if (node->kind == Logical_TreeNode_Kind::PLAN_JOIN) {
        return estimate_TreeNode_Lost_Join(node);
    }
    else if (node->kind == Logical_TreeNode_Kind::PLAN_PROJ) {
        return estimate_TreeNode_Lost_Proj(node);
    }

    return ret;
}
//这个函数没什么用了吧？涉及到的一个函数被改了，这里也不是很想改过来
Link_Lost Estimator::estimate_scan_lost(const Execution_Plan& Plan)
{
    int Page_Capacity = 0;
    int All_Record_Num = 0;
    Link_Lost ret;
    /*索引查找*/
    /*
    if (Plan.Index.size() != 0) {
        int num = estimate_record_num(Plan); //索引中可以查找到多少项
        int Page_Capacity_Index = 0;     //一个块中可以放几个index
        int Index_Page_Num = num / Page_Capacity_Index;  //需要访问几个index页
        int Page_Num = All_Record_Num / Page_Capacity; //记录总共存放在多少个块中
        ret.Disk_Scan_Lost = num / All_Record_Num * Page_Num + Index_Page_Num;
    }*/
    /*扫描查找*/
    /*
    else {
        int Page_Num = All_Record_Num / Page_Capacity;
        ret.Disk_Scan_Lost = Page_Num;
    }*/

    return ret;
}


//一元条件
int Estimator::estimate_record_num(const Condition& cond, int record_num) {
    Attr_Info attr;
    Subsystem1_Manager::BASE.lookup_Attr(cond.lhsAttr.relname, cond.lhsAttr.attrname, attr);
    Distribution distri = attr.distribution;
    table_to_record_num[cond.lhsAttr.relname] = distri.dis->rate(cond.op, *(double*)cond.rhsValue.data) * record_num;
    return table_to_record_num[cond.lhsAttr.relname];
}
//二元条件
int Estimator::estimate_record_num(const Condition& cond, int left_record_num, int right_record_num) {
    Attr_Info left_attr, right_attr;
    Subsystem1_Manager::BASE.lookup_Attr(cond.lhsAttr.relname, cond.lhsAttr.attrname, left_attr);
    Subsystem1_Manager::BASE.lookup_Attr(cond.rhsAttr.relname, cond.rhsAttr.attrname, right_attr);
    Distribution left_dis = left_attr.distribution;
    Distribution right_dis = right_attr.distribution;
    table_to_record_num[cond.lhsAttr.relname] = table_to_record_num[cond.rhsAttr.relname] =
        left_dis.dis->binary_rate(cond.op, right_dis) * left_record_num * right_record_num;
    return table_to_record_num[cond.rhsAttr.relname];
}

int Estimator::estimate_record_num(const Condition& cond) {
    if (cond.bRhsIsAttr) 
        return estimate_record_num(cond, table_to_record_num[cond.lhsAttr.relname]);
    
    return estimate_record_num(cond, table_to_record_num[cond.lhsAttr.relname],
        table_to_record_num[cond.lhsAttr.relname]);
}
