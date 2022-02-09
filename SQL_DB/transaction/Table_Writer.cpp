#include "Table_Writer.h"

// 封锁表级别
bool TableWriter::insertTuple(const Tuple& tuple, Transaction* txn) {
    string relname = tuple.getRel();
    Trid trid(relname);
    bool locked = lockManager_->LockExclusive(txn, trid);
    if (!locked) return false;

    char* record;
    tuple.toRecord(record);
    sysManager_->Insert_Reocrd(relname, record);

    LogRecord log_record(txn->getTransactionId(), txn->getPrevLSN(), LogRecordType::INSERT, tuple.getRid(),
        tuple, relname);
    lsn_t lsn = logManager_->appendLogRecord(&log_record);
    txn->setPrevLSN(lsn);

    return true;
}

bool TableWriter::deleteTuple(const Tuple& tuple, Transaction* txn) {
    string relname = tuple.getRel();
    Trid trid(relname);
    bool locked = lockManager_->LockExclusive(txn, trid);
    if (!locked) return false;

    vector<RID> rid_vec; 
    rid_vec.push_back(tuple.getRid());
    sysManager_->Delete_Record(tuple.getRel(), rid_vec);

    LogRecord log_record(txn->getTransactionId(), txn->getPrevLSN(), LogRecordType::DELETE, tuple.getRid(),
        tuple, relname);
    lsn_t lsn = logManager_->appendLogRecord(&log_record);
    txn->setPrevLSN(lsn);

    return true;
}

bool TableWriter::updateTuple(const Tuple& old_tuple, const Tuple& new_tuple, Transaction* txn) {
    assert(old_tuple.getRel() == new_tuple.getRel());
    string relname = old_tuple.getRel();
    Trid trid(relname);
    bool locked = lockManager_->LockExclusive(txn, trid);
    if (!locked) return false;

    auto attrs = sysManager_->lookup_Attrs(relname);
    char* data = new_tuple.getData();
    RID rid = old_tuple.getRid();
    // TODO 应该只需要不同值修改
    for (int i = 0, offset = 0; i < attrs.size(); i++) {
        sysManager_->Update_Record(relname, rid, attrs[i], data + offset);
        offset += attrs[i].Length;
    }

    LogRecord log_record(txn->getTransactionId(), txn->getPrevLSN(), LogRecordType::UPDATE, rid,
        old_tuple, new_tuple, relname);
    lsn_t lsn = logManager_->appendLogRecord(&log_record);
    txn->setPrevLSN(lsn);

    return true;
}