#pragma once

/**
* ����ϵͳ�Ĺ��ܸſ���˵Ӧ��ֻ������: �������� �� ��־��¼
* 
* �����Ĺ�����Ҫ����������Ӧ���ϣ�ִ������ÿ��ִ��һ��ָ��֮ǰ��������Ӧ��
* ����Ƕ�����, �õ�ǰ����ֻҪ�����������, �����д�������д�����������������ʵ��
* ����������������Ŀ�ʼ���ύ����ֹ �Լ� ÿ��д��������ϸ���� ͨ����־������ ��¼����־����
* 
* ��־���������Ը��� ��־��¼ ͨ�� �ײ�Ĵ��̹����� ������ʵ�ʵĴ���д����
* ����д�����ڼ�¼��ͬʱ, ��־������Ҳ�����ʵ�ʵ�д����
* 
* ��˸��߲��������ϵͳʱ, ����������Ӧ����, ����, �����д���������������������, ����Ƕ�������ֱ��ȥ��
* 
* ��ϵͳ������������Ĵ���ÿ�η�����ͻʱ, ֱ��������ʱ��������������ȥ, 
* ������ĳ�ͻ������ҪABORT, Ȼ��undo, ��Ϻ���������һ��
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

    // �����update��ʹ�øù��캯��
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
    /** ��ǰ����״̬. */
    TransactionState state_;

    /** �߳�ID,���ڵ��߳����� */
    std::thread::id thread_id_;

    /** ��ǰ����ID */
    txn_id_t txn_id_;

    /** The undo set of table tuples. */
    std::shared_ptr<deque<TableWriteRecord>> table_write_set_;

    /** ���ڸ��������һ��LSN */
    lsn_t prev_lsn_;

    /** ��������й�����������TRID���� */
    std::shared_ptr<set<Trid>> shared_lock_set_;

    /** ���������������������TRID���� */
    std::shared_ptr<set<Trid>> exclusive_lock_set_;
};