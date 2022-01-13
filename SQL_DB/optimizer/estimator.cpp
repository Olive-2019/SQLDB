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
        ���������Լ�rel��Ϣȷ����¼��������disk lostӦ�����ɨ��һ��
        ע�⣬filter�·��Ľ�������Ҷ�ڵ㣬Ҳ������join���
        */

        if (node->u.FILTER.rel->kind == Logical_TreeNode_Kind::PLAN_FILESCAN) {
            //���������漰�ֶεķֲ���Ϣ�Լ�������Ϣ��ȡ��ʧ
        }
        else if (node->u.FILTER.rel->kind == Logical_TreeNode_Kind::PLAN_JOIN) {
            //���������漰�ֶεķֲ���Ϣ�Լ�������Ϣ��ȡ��ʧ
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
    /*��������*/
    if (Plan.Index.size() != 0) {
        int num = estimate_record_num(Plan); //�����п��Բ��ҵ�������
        int Page_Capacity_Index = 0;     //һ�����п��Էż���index
        int Index_Page_Num = num / Page_Capacity_Index;  //��Ҫ���ʼ���indexҳ
        int Page_Num = All_Record_Num / Page_Capacity; //��¼�ܹ�����ڶ��ٸ�����
        ret.Disk_Scan_Lost = num / All_Record_Num * Page_Num + Index_Page_Num;
    }
    /*ɨ�����*/
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