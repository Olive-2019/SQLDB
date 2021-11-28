#include "estimator.h"

Estimator Estimator::estimator;

Link_Lost Estimator::estimate_link_lost_EQ(const Execution_Plan& Plan1, const Execution_Plan& Plan2, const vector<Condition>& Cond)
{
    return Link_Lost();
}

Link_Lost Estimator::estimate_link_lost_LT(const Execution_Plan& Plan1, const Execution_Plan& Plan2, const vector<Condition>& Cond)
{
    return Link_Lost();
}

Link_Lost Estimator::estimate_link_lost_GT(const Execution_Plan& Plan1, const Execution_Plan& Plan2, const vector<Condition>& Cond)
{
    return Link_Lost();
}

Link_Lost Estimator::estimate_order_lost(const vector<Execution_Plan>& Plan_Order)
{
    return Link_Lost();
}

Link_Lost Estimator::estimate_link_lost(const Execution_Plan& Plan1, const Execution_Plan& Plan2, const vector<Condition>& Cond)
{
    
    return Link_Lost();
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