#include "Table_Writer.h"

// 封锁表级别, 要达到封锁更细粒度可能需要直接在底层实现, 但是目前封装的东西很多不好做
bool TableWriter::insertTuple(Tuple& tuple, Transaction* txn, LockManager* lock_manager, LogManager* log_manager) {
    string relname = tuple.getRel();

    if (lock_manager) {
        Trid trid(relname);
        bool locked = lock_manager->LockExclusive(txn, trid);
        if (!locked) return false;
    }

    RID rid = Subsystem1_Manager::mgr.Insert_Reocrd(relname, tuple.getData());

    if (lock_manager) {
        tuple.setRid(rid);
        LogRecord log_record(txn->getTransactionId(), txn->getPrevLSN(), LogRecordType::INSERT, tuple.getRid(),
            tuple, relname);
        lsn_t lsn = log_manager->appendLogRecord(&log_record);
        txn->setPrevLSN(lsn);
    }
    return true;
}

bool TableWriter::deleteTuple(const Tuple& tuple, Transaction* txn, LockManager* lock_manager, LogManager* log_manager) {
    string relname = tuple.getRel();
    Trid trid(relname);
    bool locked = lock_manager->LockExclusive(txn, trid);
    if (!locked) return false;

    vector<RID> rid_vec; 
    rid_vec.push_back(tuple.getRid());
    Subsystem1_Manager::mgr.Delete_Record(tuple.getRel(), rid_vec);

    LogRecord log_record(txn->getTransactionId(), txn->getPrevLSN(), LogRecordType::DELETE, tuple.getRid(),
        tuple, relname);
    lsn_t lsn = log_manager->appendLogRecord(&log_record);
    txn->setPrevLSN(lsn);

    return true;
}

bool TableWriter::updateTuple(const Tuple& old_tuple, const Tuple& new_tuple, Transaction* txn, 
    LockManager* lock_manager, LogManager* log_manager) {

    assert(old_tuple.getRel() == new_tuple.getRel());

    string relname = old_tuple.getRel();

    if (lock_manager) {
        Trid trid(relname);
        bool locked = lock_manager->LockExclusive(txn, trid);
        if (!locked) return false;
    }

    auto attrs = Subsystem1_Manager::mgr.lookup_Attrs(relname);
    char* data = new_tuple.getData();
    RID rid = old_tuple.getRid();
    // TODO 应该只需要不同值修改
    for (int i = 0, offset = 0; i < attrs.size(); i++) {
        Subsystem1_Manager::mgr.Update_Record(relname, rid, attrs[i], data + offset);
        offset += attrs[i].Length;
    }

    if (lock_manager) {
        LogRecord log_record(txn->getTransactionId(), txn->getPrevLSN(), LogRecordType::UPDATE, rid,
            old_tuple, new_tuple, relname);
        lsn_t lsn = log_manager->appendLogRecord(&log_record);
        txn->setPrevLSN(lsn);
    }
    return true;
}