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
    assert(txn->getSharedLockSet()->count(trid) == 0); // ȷ��ͬһ�������ظ�������

    LockRequest request(txn->getTransactionId(), LockMode::SHARED);
    if (lock_table_.count(trid) == 0) {
        lock_table_[trid].oldest_ = txn->getTransactionId();
        lock_table_[trid].request_queue_.push_back(request);
    }
    else {
        // ����ȴ�������û��������,�Ͳ���Ҫ������ϳ̶�,���������ᱻ��Ȩ
        // ������ֵȴ�����Ͳ�������
        if (lock_table_[trid].exclusive_cnt_ != 0 && txn->getTransactionId() > lock_table_[trid].oldest_) {
            txn->setState(TransactionState::ABORTED); // ��������õ��������ȼ������Ը��ñ�������
            return false;
        }
        else {
            for (auto& it : lock_table_[trid].request_queue_) {
                // ���µ�д��������ȴ�ڶ���ǰ��,д����������Ҫ����
                if (it.lock_mode_ == LockMode::EXCLUSIVE && it.txn_id_ > txn->getTransactionId()) {
                    TransactionManager::getTransaction(it.txn_id_)->setState(TransactionState::ABORTED);
                }
            }
            // ����������,�ȴ���Ȩ
            lock_table_[trid].oldest_ = txn->getTransactionId();
            lock_table_[trid].request_queue_.push_back(request);
        }
    }

    // ͨ������: ǰ��ȫ������Ȩ��shared����
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

    // �����Ѿ������˱仯,���������������л�����
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
    assert(txn->getExclusiveLockSet()->count(rid) == 0); // ȷ��ͬһ�������ظ�������

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
                if (it.txn_id_ > txn->getTransactionId()) { // ���µ���������ȴ��ǰ��,��Ҫ����
                    TransactionManager::getTransaction(it.txn_id_)->setState(TransactionState::ABORTED);
                }
            }
            lock_table_[rid].oldest_ = txn->getTransactionId();
            lock_table_[rid].request_queue_.push_back(request);
        }
    }

    // ͨ����������ǰ����֮ǰû���κ�����Ȩ������
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

    // ��Ȩһ�������������۹������������������������л����ȡ�����Բ���Ҫnotify
    return true;
}

bool LockManager::LockUpgrade(Transaction* txn, const Trid& trid) {
    std::unique_lock<std::mutex> lock(latch_);
    if (txn->getState() == TransactionState::ABORTED) {
        return false;
    }
    assert(txn->getState() == TransactionState::GROWING); // ֻ���������׶�����
    assert(txn->getSharedLockSet()->count(trid) != 0); // ֻ���ɹ���������Ϊ������

    // ͨ����������Ҫ���������shared������Ψһһ��granted����
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

    if (strict_2PL_) { // �ϸ����׶�
        if (txn->getState() != TransactionState::ABORTED ||
            txn->getState() != TransactionState::COMMITTED) {
            txn->setState(TransactionState::ABORTED);
            return false;
        }
    }
    // ��֤�˺�һ���������׶�
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
    // ����oldest_
    for (auto it = lock_table_[trid].request_queue_.begin();
        it != lock_table_[trid].request_queue_.end(); ++it) {
        if (it->txn_id_ < lock_table_[trid].oldest_) {
            lock_table_[trid].oldest_ = it->txn_id_;
        }
    }
    cv_.notify_all();
    return true;
}