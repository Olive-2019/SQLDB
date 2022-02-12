#pragma once
#include "../storage/Subsystem1_Manager.h"
#include "Lock_Manager.h"
#include "Log_Manager.h"

class TableWriter{
public:
    TableWriter() = default;

    // ָ�����͵ı���ֻ����������־REDO/UNDOʱ�Ż�Ϊ��, ���Խ������ΪREDO��UNDO�ı�־

    // ���������, tuple�е�rid��Ϊ��
    static bool insertTuple(Tuple& tuple, Transaction* txn, LockManager* lock_manager, LogManager* log_manager);

    // Ҫ�Ȳ�ѯ��rid, tuple�е�rid���ܿ�
    static bool deleteTuple(const Tuple& tuple, Transaction* txn, LockManager* lock_manager, LogManager* log_manager);

    // Ҫ�Ȳ�ѯ��ԭ��¼, rid��Ϊ��
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