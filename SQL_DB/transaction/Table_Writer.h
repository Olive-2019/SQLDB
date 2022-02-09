#pragma once
#include "../storage/Subsystem1_Manager.h"
#include "Lock_Manager.h"
#include "Log_Manager.h"

class TableWriter{
public:
    TableWriter(Subsystem1_Manager* sys_manager, LockManager* lock_manager, LogManager* log_manager)
        :sysManager_(sys_manager), lockManager_(lock_manager), logManager_(log_manager) {}

    bool insertTuple(const Tuple& tuple, Transaction* txn);

    bool deleteTuple(const Tuple& tuple, Transaction* txn);

    bool updateTuple(const Tuple& old_tuple, const Tuple& new_tuple, Transaction* txn);

private:
    Subsystem1_Manager* sysManager_;
    
    LockManager* lockManager_;

    LogManager* logManager_;

};