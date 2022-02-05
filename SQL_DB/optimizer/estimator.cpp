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

    int record_length = 0;
    if (attrs.size()) record_length = attrs.back().Offset + attrs.back().Length;
    ret.Memory_Scan_Lost = 0;
    ret.record_num = temp.Record_Num;
    ret.Disk_Scan_Lost = (ret.record_num * record_length) / Global_Paras::Block_Size;
    return ret;
}

Link_Lost Estimator::estimate_TreeNode_Lost_Filter(Logical_TreeNode* node)
{
    Link_Lost ret;
    Link_Lost temp = estimate_TreeNode_Lost(node->u.FILTER.rel);
    ret = temp;
    /*
    ���������Լ�rel��Ϣȷ����¼��������disk lostӦ�����ɨ��һ��
    ע�⣬filter�·��Ľ�������Ҷ�ڵ㣬Ҳ������join����filter
    */
    
    ret.record_num = estimate_record_num(*node->u.FILTER.expr_filter, ret.record_num);
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
    cout << "attr_num==" << attr_num << endl;
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
//�������ûʲô���˰ɣ��漰����һ�����������ˣ�����Ҳ���Ǻ���Ĺ���
Link_Lost Estimator::estimate_scan_lost(const Execution_Plan& Plan)
{
    int Page_Capacity = 0;
    int All_Record_Num = 0;
    Link_Lost ret;
    /*��������*/
    /*
    if (Plan.Index.size() != 0) {
        int num = estimate_record_num(Plan); //�����п��Բ��ҵ�������
        int Page_Capacity_Index = 0;     //һ�����п��Էż���index
        int Index_Page_Num = num / Page_Capacity_Index;  //��Ҫ���ʼ���indexҳ
        int Page_Num = All_Record_Num / Page_Capacity; //��¼�ܹ�����ڶ��ٸ�����
        ret.Disk_Scan_Lost = num / All_Record_Num * Page_Num + Index_Page_Num;
    }*/
    /*ɨ�����*/
    /*
    else {
        int Page_Num = All_Record_Num / Page_Capacity;
        ret.Disk_Scan_Lost = Page_Num;
    }*/

    return ret;
}



int Estimator::estimate_record_num(const Condition& cond, int record_num) {

    if (cond.bRhsIsAttr) {
        Attr_Info left_attr, right_attr;
        Subsystem1_Manager::BASE.lookup_Attr(cond.lhsAttr.relname, cond.lhsAttr.attrname, left_attr);
        Subsystem1_Manager::BASE.lookup_Attr(cond.rhsAttr.relname, cond.rhsAttr.attrname, right_attr);
        Distribution left_dis = left_attr.distribution;
        Distribution right_dis = right_attr.distribution;
        return left_dis.dis->binary_rate(cond.op, right_dis) * record_num;
    }
    else {
        Attr_Info attr;
        Subsystem1_Manager::BASE.lookup_Attr(cond.lhsAttr.relname, cond.lhsAttr.attrname, attr);
        Distribution distri = attr.distribution;
        return distri.dis->rate(cond.op, *(double*)cond.rhsValue.data) * record_num;
    }
    
}


