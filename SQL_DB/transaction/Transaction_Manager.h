#pragma once

#include "Lock_Manager.h"
#include "Log_Manager.h"
#include "Table_Writer.h"

#include <atomic>
#include <shared_mutex>
#include <cassert>
#include <set>

class TransactionManager {
public:
    // 方便全局管理事务
    static std::map<txn_id_t, Transaction*> txn_map;
    static std::shared_mutex txn_map_mutex;

    TransactionManager(LockManager* lock_manager, LogManager* log_manager)
        : next_txn_id_(0), lock_manager_(lock_manager), log_manager_(log_manager) {}

    static Transaction* getTransaction(txn_id_t txn_id) {
        TransactionManager::txn_map_mutex.lock_shared();
        assert(TransactionManager::txn_map.find(txn_id) != TransactionManager::txn_map.end());
        auto* res = TransactionManager::txn_map[txn_id];
        assert(res != nullptr);
        TransactionManager::txn_map_mutex.unlock_shared();
        return res;
    }

    Transaction* begin(Transaction* txn = nullptr);

    void commit(Transaction* txn);

    void abort(Transaction* txn);

    void write(TableWriteRecord write_record, Transaction* txn);

private:

    std::atomic<txn_id_t> next_txn_id_;

    LockManager* lock_manager_;

    LogManager* log_manager_;
};