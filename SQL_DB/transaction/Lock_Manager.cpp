#include "lock_manager.h"
#include "transaction_manager.h"
#include <cassert>

bool LockManager::LockShared(Transaction* txn, const Trid& trid) {
    std::unique_lock<mutex> lock(latch_);
    if (txn->getState() == TransactionState::ABORTED) {
        return false;
    }
    if (txn->getState() != TransactionState::GROWING || txn->getSharedLockSet()->count(trid) != 0) {
        txn->setState(TransactionState::ABORTED);
        return false;
    }
    assert(txn->getState() == TransactionState::GROWING);
    assert(txn->getSharedLockSet()->count(trid) == 0); // 确保同一个事务不重复申请锁

    LockRequest request(txn->getTransactionId(), LockMode::SHARED);
    if (lock_table_.count(trid) == 0) {
        lock_table_[trid].oldest_ = txn->getTransactionId();
        lock_table_[trid].request_queue_.push_back(request);
    }
    else {
        // 如果等待队列中没有排他锁,就不需要检测新老程度,共享锁都会被授权
        // 不会出现等待情况就不会死锁
        if (lock_table_[trid].exclusive_cnt_ != 0 && txn->getTransactionId() > lock_table_[trid].oldest_) {
            txn->setState(TransactionState::ABORTED); // 让生存最久的事务优先加锁可以更好避免死锁
            return false;
        }
        else {
            for (auto& it : lock_table_[trid].request_queue_) {
                // 更新的写操作申请却在读的前面,写操作事务需要重启
                if (it.lock_mode_ == LockMode::EXCLUSIVE && it.txn_id_ > txn->getTransactionId()) {
                    TransactionManager::getTransaction(it.txn_id_)->setState(TransactionState::ABORTED);
                }
            }
            // 申请加入队列,等待授权
            lock_table_[trid].oldest_ = txn->getTransactionId();
            lock_table_[trid].request_queue_.push_back(request);
        }
    }

    // 通过条件: 前面全是已授权的shared请求
    LockRequest* cur = nullptr;
    cv_.wait(lock, [&]() -> bool {
        for (auto it = lock_table_[trid].request_queue_.begin();
            it != lock_table_[trid].request_queue_.end(); ++it) {
            if (it->txn_id_ != txn->getTransactionId()) {
                if (it->lock_mode_ != LockMode::SHARED || it->granted_) {
                    return false;
                }
            }
            else {
                cur = &(*it);
                break;
            }
        }
        return true;
        });

    cur->granted_ = true;
    txn->getSharedLockSet()->insert(trid);

    // 条件已经发生了变化,其他共享锁请求有机会获得
    cv_.notify_one();
    return true;
}

bool LockManager::LockExclusive(Transaction* txn, const Trid& rid) {
    std::unique_lock<std::mutex> lock(latch_);
    if (txn->getState() == TransactionState::ABORTED) {
        return false;
    }
    if (txn->getState() != TransactionState::GROWING || txn->getExclusiveLockSet()->count(rid) != 0) {
        txn->setState(TransactionState::ABORTED);
        return false;
    }
    assert(txn->getState() == TransactionState::GROWING);
    assert(txn->getExclusiveLockSet()->count(rid) == 0); // 确保同一个事务不重复申请锁

    LockRequest request(txn->getTransactionId(), LockMode::EXCLUSIVE);
    if (lock_table_.count(rid) == 0) {
        lock_table_[rid].oldest_ = txn->getTransactionId();
        lock_table_[rid].request_queue_.push_back(request);
    }
    else {
        if (txn->getTransactionId() > lock_table_[rid].oldest_) {
            txn->setState(TransactionState::ABORTED);
            return false;
        }
        else {
            for (auto& it : lock_table_[rid].request_queue_) {
                if (it.txn_id_ > txn->getTransactionId()) { // 更新的事务申请却在前面,需要重启
                    TransactionManager::getTransaction(it.txn_id_)->setState(TransactionState::ABORTED);
                }
            }
            lock_table_[rid].oldest_ = txn->getTransactionId();
            lock_table_[rid].request_queue_.push_back(request);
        }
    }

    // 通过条件：当前请求之前没有任何已授权的请求
    LockRequest* cur = nullptr;
    cv_.wait(lock, [&]() -> bool {
        for (auto it = lock_table_[rid].request_queue_.begin();
            it != lock_table_[rid].request_queue_.end(); ++it) {
            if (it->txn_id_ != txn->getTransactionId()) {
                if (it->granted_) {
                    return false;
                }
            }
            else {
                cur = &(*it);
                break;
            }
        }
        return true;
        });

    cur->granted_ = true;
    lock_table_[rid].exclusive_cnt_++;
    txn->getExclusiveLockSet()->insert(rid);

    // 授权一个排它锁后，无论共享锁还是排它锁都不可能有机会获取，所以不需要notify
    return true;
}

bool LockManager::LockUpgrade(Transaction* txn, const Trid& trid) {
    std::unique_lock<std::mutex> lock(latch_);
    if (txn->getState() == TransactionState::ABORTED) {
        return false;
    }
    assert(txn->getState() == TransactionState::GROWING); // 只能在增长阶段升级
    assert(txn->getSharedLockSet()->count(trid) != 0); // 只能由共享锁升级为排他锁

    // 通过条件：想要升级的这个shared请求是唯一一个granted请求
    cv_.wait(lock, [&]() -> bool {
        for (auto it = lock_table_[trid].request_queue_.begin();
            it != lock_table_[trid].request_queue_.end(); ++it) {
            if (it == lock_table_[trid].request_queue_.begin() && it->txn_id_ != txn->getTransactionId()) {
                return false;
            }
            if (it != lock_table_[trid].request_queue_.begin() && it->granted_) {
                return false;
            }
        }
        return true;
        });

    auto cur = lock_table_[trid].request_queue_.begin();
    cur->lock_mode_ = LockMode::EXCLUSIVE;
    lock_table_[trid].exclusive_cnt_++;
    txn->getSharedLockSet()->erase(trid);
    txn->getExclusiveLockSet()->insert(trid);
    return true;
}

bool LockManager::Unlock(Transaction* txn, const Trid& trid) {
    std::unique_lock<std::mutex> latch(latch_);
    assert(txn->getSharedLockSet()->count(trid) || txn->getExclusiveLockSet()->count(trid));

    if (strict_2PL_) { // 严格两阶段
        if (txn->getState() != TransactionState::ABORTED ||
            txn->getState() != TransactionState::COMMITTED) {
            txn->setState(TransactionState::ABORTED);
            return false;
        }
    }
    // 保证此后一定是缩减阶段
    if (txn->getState() == TransactionState::GROWING) {
        txn->setState(TransactionState::SHRINKING);
    }

    for (auto it = lock_table_[trid].request_queue_.begin();
        it != lock_table_[trid].request_queue_.end(); ++it) {
        if (it->txn_id_ == txn->getTransactionId()) {
            if (it->lock_mode_ == LockMode::SHARED) {
                txn->getSharedLockSet()->erase(trid);
            }
            else {
                txn->getExclusiveLockSet()->erase(trid);
                lock_table_[trid].exclusive_cnt_--;
            }
            lock_table_[trid].request_queue_.erase(it);
            break;
        }
    }
    // 更新oldest_
    for (auto it = lock_table_[trid].request_queue_.begin();
        it != lock_table_[trid].request_queue_.end(); ++it) {
        if (it->txn_id_ < lock_table_[trid].oldest_) {
            lock_table_[trid].oldest_ = it->txn_id_;
        }
    }
    cv_.notify_all();
    return true;
}