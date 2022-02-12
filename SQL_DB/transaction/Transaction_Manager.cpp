#include "transaction_manager.h"

#include <cassert>

std::map<txn_id_t, Transaction*> TransactionManager::txn_map = {};
std::shared_mutex TransactionManager::txn_map_mutex = {};

Transaction* TransactionManager::begin(Transaction* txn) {
    txn_map_mutex.lock();
    if (txn == nullptr) txn = new Transaction(next_txn_id_++);

    // TODO: write log and update transaction's prev_lsn here
    auto logRecord = LogRecord(txn->getTransactionId(), txn->getPrevLSN(), LogRecordType::BEGIN);
    log_manager_->appendLogRecord(&logRecord);

    txn_map[txn->getTransactionId()] = txn;
    txn_map_mutex.unlock();
    return txn;
}

void TransactionManager::commit(Transaction* txn) {
    txn_map_mutex.lock();
    txn->setState(TransactionState::COMMITTED);

    // TODO: write log and update transaction's prev_lsn here
    auto logRecord = LogRecord(txn->getTransactionId(), txn->getPrevLSN(), LogRecordType::COMMIT);
    log_manager_->appendLogRecord(&logRecord);
    log_manager_->forceFlush();

    // release all the lock
    std::set<Trid> lock_set;
    auto sharedLockSet = txn->getSharedLockSet();
    for (auto item : *sharedLockSet) {
        lock_set.emplace(item);
    }
    auto exclusiveLockSet = txn->getExclusiveLockSet();
    for (auto item : *exclusiveLockSet) {
        lock_set.emplace(item);
    }
    // release all the lock
    for (auto locked_trid : lock_set) {
        lock_manager_->Unlock(txn, locked_trid);
    }

    auto it = txn_map.find(txn->getTransactionId());
    txn_map.erase(it);
    txn_map_mutex.unlock();
}

void TransactionManager::abort(Transaction* txn) {
    txn->setState(TransactionState::ABORTED);
    // rollback before releasing lock
    auto write_set = txn->getWriteSet();
    while (!write_set->empty()) {
        auto& item = write_set->back();
        auto relname = item.relname_;
        if (item.wtype_ == WType::DELETE) {
            cout << ("rollback delete\n");
            // 回滚操作 RollbackDelete(item.rid_, txn);
            Tuple tuple(relname, item.values_, item.rid_);
            TableWriter::insertTuple(tuple, txn, lock_manager_, log_manager_);
        }
        else if (item.wtype_ == WType::INSERT) {
            cout << ("rollback insert\n");
            // 回滚操作 ApplyDelete(item.rid_, txn);
            Tuple tuple(relname, item.values_, item.rid_);
            TableWriter::deleteTuple(tuple, txn, lock_manager_, log_manager_);
        }
        else if (item.wtype_ == WType::UPDATE) {
            cout << ("rollback update\n");
            // 回滚操作 UpdateTuple(item.tuple_, item.rid_, txn);
            Tuple old_tuple(relname, item.values_, item.rid_);
            Tuple new_tuple(relname, item.new_values_, item.rid_);
            TableWriter::updateTuple(new_tuple, old_tuple, txn, lock_manager_, log_manager_);
        }
        write_set->pop_back();
    }
    write_set->clear();

    // TODO: write log and update transaction's prev_lsn here
    auto logRecord = LogRecord(txn->getTransactionId(), txn->getPrevLSN(), LogRecordType::ABORT);
    log_manager_->appendLogRecord(&logRecord);
    log_manager_->forceFlush();

    // release all the lock
    std::set<Trid> lock_set;
    auto sharedLockSet = txn->getSharedLockSet();
    for (auto item : *sharedLockSet) {
        lock_set.emplace(item);
    }
    auto exclusiveLockSet = txn->getExclusiveLockSet();
    for (auto item : *exclusiveLockSet) {
        lock_set.emplace(item);
    }
    // release all the lock
    for (auto locked_trid : lock_set) {
        lock_manager_->Unlock(txn, locked_trid);
    }
}

void TransactionManager::write(TableWriteRecord write_record, Transaction* txn) {
    txn->AppendTableWriteRecord(write_record);

    string relname = write_record.relname_;
    RID rid = write_record.rid_;
    WType wtype = write_record.wtype_;

    if (wtype == WType::DELETE) {
        cout << ("delete\n");
        Tuple tuple(relname, write_record.values_, rid);
        TableWriter::deleteTuple(tuple, txn, lock_manager_, log_manager_);
    }
    else if (wtype == WType::INSERT) {
        cout << ("insert\n");
        Tuple tuple(relname, write_record.values_, rid);
        TableWriter::insertTuple(tuple, txn, lock_manager_, log_manager_);
    }
    else if (wtype == WType::UPDATE) {
        cout << ("update\n");
        Tuple old_tuple(relname, write_record.values_, rid);
        Tuple new_tuple(relname, write_record.new_values_, rid);
        TableWriter::updateTuple(old_tuple, new_tuple, txn, lock_manager_, log_manager_);
    }
}