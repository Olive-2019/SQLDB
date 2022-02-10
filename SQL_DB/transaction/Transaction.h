#pragma once

/**
* 事务系统的功能概况来说应该只有两个: 并发控制 和 日志记录
* 
* 并发的功能主要体现在锁的应用上：执行器在每次执行一条指令之前会申请相应锁
* 如果是读操作, 让当前事务只要申请读锁即可, 如果是写操作则把写操作交给事务管理器实现
* 事务管理器会把事务的开始、提交、中止 以及 每次写操作的详细内容 通过日志管理器 记录在日志里面
* 
* 日志管理器可以根据 日志记录 通过 底层的磁盘管理器 来进行实际的磁盘写操作
* 所以写操作在记录的同时, 日志管理器也会进行实际的写操作
* 
* 因此更高层调用事务系统时, 仅需申请相应的锁, 接着, 如果是写操作传给事务管理器来做, 如果是读操作就直接去读
* 
* 该系统关于死锁问题的处理：每次发生冲突时, 直接让生存时间最长的事务进行下去, 
* 而年轻的冲突事务需要ABORT, 然后undo, 完毕后再重新做一遍
*/

#include "../global.h" // RID
#include "../parser/parser_interp.h" // Value
#include "Tuple.h"

#include <thread>
#include <deque>
#include <map>
#include <set>

/**
 * Transaction states for 2PL:
 *
 *     _________________________
 *    |                         v
 * GROWING -> SHRINKING -> COMMITTED   ABORTED
 *    |__________|________________________^
 *
 **/

enum class TransactionState { GROWING, SHRINKING, COMMITTED, ABORTED };

// Default IsolationLevel = Repeatable Read

/**
 * Type of write operation.
 */
enum class WType { INSERT = 0, DELETE, UPDATE };

/**
 * WriteRecord tracks information related to a write.
 */
//class TableWriteRecord {
//public:
//    TableWriteRecord(RID rid, WType wtype, const vector<Value> values, const string& table)
//        : rid_(rid), wtype_(wtype), values_(values), table_(table) {}
//
//    RID rid_;
//    WType wtype_;
//    vector<Value> values_;
//    string table_;
//};

class TableWriteRecord {
public:
    TableWriteRecord(RID rid, WType wtype, const string& relname, const vector<Value>& values)
        : rid_(rid), wtype_(wtype), values_(values), relname_(relname) {}

    // 如果是update会使用该构造函数
    TableWriteRecord(RID rid, WType wtype, const string& relname, const vector<Value>& old_values, const vector<Value>& new_values)
        : rid_(rid), wtype_(wtype), values_(old_values), relname_(relname), new_values_(new_values) {}

    RID rid_;
    WType wtype_;
    string relname_;
    vector<Value> values_;
    vector<Value> new_values_;
};

class Transaction {
public:
    explicit Transaction(txn_id_t txn_id)
        : state_(TransactionState::GROWING),
        thread_id_(this_thread::get_id()),
        txn_id_(txn_id),
        prev_lsn_(INVALID_LSN),
        shared_lock_set_{ new std::set<Trid> },
        exclusive_lock_set_{ new std::set<Trid> }
    {
        table_write_set_ = make_shared<std::deque<TableWriteRecord>>();
    }

    ~Transaction() = default;

    inline std::thread::id getThreadId() const { return thread_id_; }

    inline txn_id_t getTransactionId() const { return txn_id_; }

    inline std::shared_ptr<std::deque<TableWriteRecord>> getWriteSet() { return table_write_set_; }

    inline void AppendTableWriteRecord(const TableWriteRecord& write_record) { table_write_set_->push_back(write_record); }

    inline std::shared_ptr<std::set<Trid>> getSharedLockSet() { return shared_lock_set_; }

    inline std::shared_ptr<std::set<Trid>> getExclusiveLockSet() { return exclusive_lock_set_; }

    bool IsSharedLocked(const Trid& trid) { return shared_lock_set_->find(trid) != shared_lock_set_->end(); }

    bool IsExclusiveLocked(const Trid& trid) { return exclusive_lock_set_->find(trid) != exclusive_lock_set_->end(); }

    inline TransactionState getState() { return state_; }

    inline void setState(TransactionState state) { state_ = state; }

    inline lsn_t getPrevLSN() { return prev_lsn_; }

    inline void setPrevLSN(lsn_t prev_lsn) { prev_lsn_ = prev_lsn; }

private:
    /** 当前事务状态. */
    TransactionState state_;

    /** 线程ID,用于单线程事务 */
    std::thread::id thread_id_;

    /** 当前事务ID */
    txn_id_t txn_id_;

    /** The undo set of table tuples. */
    std::shared_ptr<deque<TableWriteRecord>> table_write_set_;

    /** 属于该事务的上一个LSN */
    lsn_t prev_lsn_;

    /** 该事务持有共享锁的数据TRID集合 */
    std::shared_ptr<set<Trid>> shared_lock_set_;

    /** 该事务持有排他锁的数据TRID集合 */
    std::shared_ptr<set<Trid>> exclusive_lock_set_;
};