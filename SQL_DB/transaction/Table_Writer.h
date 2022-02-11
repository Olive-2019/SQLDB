#pragma once
#include "../storage/Subsystem1_Manager.h"
#include "Lock_Manager.h"
#include "Log_Manager.h"

class TableWriter{
public:
    TableWriter() = default;

    // ָ�����͵ı���ֻ����������־REDO/UNDOʱ�Ż�Ϊ��, ���Խ������ΪREDO��UNDO�ı�־

    // ���������, tuple�е�rid��Ϊ��
    bool insertTuple(const Tuple& tuple, Transaction* txn, LockManager* lock_manager, LogManager* log_manager);

    // Ҫ�Ȳ�ѯ��rid, tuple�е�rid���ܿ�
    bool deleteTuple(const Tuple& tuple, Transaction* txn, LockManager* lock_manager, LogManager* log_manager);

    // Ҫ�Ȳ�ѯ��ԭ��¼, rid��Ϊ��
    bool updateTuple(const Tuple& old_tuple, const Tuple& new_tuple, Transaction* txn, 
        LockManager* lock_manager, LogManager* log_manager);
};