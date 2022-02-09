#pragma once

#include "Transaction.h"

#include <condition_variable>
#include <map>

class LockManager {
    enum class LockMode { SHARED, EXCLUSIVE };

    class LockRequest {
    public:
        LockRequest(txn_id_t txn_id, LockMode lock_mode) : txn_id_(txn_id), lock_mode_(lock_mode), granted_(false) {}

        txn_id_t txn_id_;
        LockMode lock_mode_;
        bool granted_;
    };

    class LockRequestQueue {
    public:
        std::list<LockRequest> request_queue_;
        // 排他锁申请的请求数目
        int exclusive_cnt_ = 0;
        // 生存最久的事务
        txn_id_t oldest_ = INVALID_TXN_ID;
    };

public:
    LockManager(bool strict_2PL_ = false) : strict_2PL_(strict_2PL_) {};

    ~LockManager() = default;

    //bool LockShared(Transaction* txn, const RID& rid); --- 废弃
    bool LockShared(Transaction* txn, const Trid& trid);

    //bool LockExclusive(Transaction* txn, const RID& rid);
    bool LockExclusive(Transaction* txn, const Trid& trid);

    //bool LockUpgrade(Transaction* txn, const RID& rid);
    bool LockUpgrade(Transaction* txn, const Trid& trid);

    //bool Unlock(Transaction* txn, const RID& rid);
    bool Unlock(Transaction* txn, const Trid& trid);

private:
    bool strict_2PL_;

    std::mutex latch_;

    std::condition_variable cv_;

    // 为什么不用unordered_map/set？ 是因为visual studio不支持
    /** Lock table for lock requests. */
    //std::map<RID, LockRequestQueue> lock_table_;
    std::map<Trid, LockRequestQueue> lock_table_;
};