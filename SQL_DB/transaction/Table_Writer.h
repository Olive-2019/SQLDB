#pragma once
#include "../storage/Subsystem1_Manager.h"
#include "Lock_Manager.h"
#include "Log_Manager.h"

class TableWriter{
public:
    TableWriter() = default;

    // 指针类型的变量只有在整个日志REDO/UNDO时才会为空, 可以将这个看为REDO和UNDO的标志

    // 表封锁级别, tuple中的rid可为空
    bool insertTuple(const Tuple& tuple, Transaction* txn, LockManager* lock_manager, LogManager* log_manager);

    // 要先查询到rid, tuple中的rid不能空
    bool deleteTuple(const Tuple& tuple, Transaction* txn, LockManager* lock_manager, LogManager* log_manager);

    // 要先查询到原记录, rid不为空
    bool updateTuple(const Tuple& old_tuple, const Tuple& new_tuple, Transaction* txn, 
        LockManager* lock_manager, LogManager* log_manager);
};