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