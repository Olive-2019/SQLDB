#pragma once
#include "../storage/Subsystem1_Manager.h"
#include "Lock_Manager.h"
#include "Log_Manager.h"

class TableWriter{
public:
    TableWriter() = default;

    // 指针类型的变量只有在整个日志REDO/UNDO时才会为空, 可以将这个看为REDO和UNDO的标志

    // 表封锁级别, tuple中的rid可为空
    static bool insertTuple(Tuple& tuple, Transaction* txn, LockManager* lock_manager, LogManager* log_manager);

    // 要先查询到rid, tuple中的rid不能空
    static bool deleteTuple(const Tuple& tuple, Transaction* txn, LockManager* lock_manager, LogManager* log_manager);

    // 要先查询到原记录, rid不为空
    static bool updateTuple(const Tuple& old_tuple, const Tuple& new_tuple, Transaction* txn, 
        LockManager* lock_manager, LogManager* log_manager);

    static vector<Value> GetValues(string rel, char* data) {
        auto attrs = Subsystem1_Manager::mgr.lookup_Attrs(rel);
        int32_t offset = 0;
        vector<Value> values;
        for (const auto& attr : attrs) {
            Value value;
            value.type = attr.type;
            value.data = data + offset;
            //memcpy(value.data, data + offset, attr.Length);
            values.push_back(value);
            offset += attr.Length;
        }
        return values;
    }
};