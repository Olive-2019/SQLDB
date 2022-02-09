#pragma once

#include "../global.h" // RID
#include "Tuple.h"

enum class LogRecordType {
    INVALID = 0,
    INSERT,
    DELETE,
    UPDATE,
    BEGIN,
    COMMIT,
    ABORT
};

/**
 * For every write operation on the table page, you should write ahead a corresponding log record.
 *
 * For EACH log record, HEADER is like (5 fields in common, 20 bytes in total).
 *---------------------------------------------
 * | size | LSN | transID | prevLSN | LogType |
 *---------------------------------------------
 * For insert type log record
 *-------------------------------------------------------------------------
 * | HEADER | tuple_rid | tuple_size | tuple_data(char[] array) | relname |
 *-------------------------------------------------------------------------
 * For delete type (including markdelete, rollbackdelete, applydelete)
 *-------------------------------------------------------------------------
 * | HEADER | tuple_rid | tuple_size | tuple_data(char[] array) | relname |
 *-------------------------------------------------------------------------
 * For update type log record
 *---------------------------------------------------------------------------------------------
 * | HEADER | tuple_rid | tuple_size | old_tuple_data | tuple_size | new_tuple_data | relname |
 *---------------------------------------------------------------------------------------------
 */

class LogRecord {
    friend class LogManager;

public:
    LogRecord() = default;

    // constructor for Transaction type(BEGIN/COMMIT/ABORT)
    LogRecord(txn_id_t txn_id, lsn_t prev_lsn, LogRecordType log_record_type)
        : size_(HEADER_SIZE), txn_id_(txn_id), prev_lsn_(prev_lsn), log_record_type_(log_record_type) {}

    // constructor for INSERT/DELETE type
    LogRecord(txn_id_t txn_id, lsn_t prev_lsn, LogRecordType log_record_type, const RID& rid, const Tuple& tuple, string relname)
        : txn_id_(txn_id), prev_lsn_(prev_lsn), log_record_type_(log_record_type), relname_(relname) {
        if (log_record_type == LogRecordType::INSERT) {
            insert_rid_ = rid;
            insert_tuple_ = tuple;
        }
        else {
            delete_rid_ = rid;
            delete_tuple_ = tuple;
        }

        size_ = HEADER_SIZE + sizeof(RID) + sizeof(int32_t) + tuple.getLength() + tuple.getRel().length();
    }

    // constructor for UPDATE type
    LogRecord(txn_id_t txn_id, lsn_t prev_lsn, LogRecordType log_record_type, const RID& update_rid,
        const Tuple& old_tuple, const Tuple& new_tuple, string relname)
        : txn_id_(txn_id),
        prev_lsn_(prev_lsn),
        log_record_type_(log_record_type),
        update_rid_(update_rid),
        old_tuple_(old_tuple),
        new_tuple_(new_tuple), 
        relname_(relname){
        // calculate log record size
        size_ = HEADER_SIZE + sizeof(RID) + old_tuple.getLength() + new_tuple.getLength() + sizeof(txn_id_t) + sizeof(lsn_t);
    }

    ~LogRecord() = default;

    inline Tuple& getDeleteTuple() { return delete_tuple_; }

    inline RID& getDeleteRID() { return delete_rid_; }

    inline Tuple& getInsertTuple() { return insert_tuple_; }

    inline RID& getInsertRID() { return insert_rid_; }

    inline Tuple& getOriginalTuple() { return old_tuple_; }

    inline Tuple& getUpdateTuple() { return new_tuple_; }

    inline RID& getUpdateRID() { return update_rid_; }

    inline int32_t getSize() { return size_; }

    inline lsn_t getLSN() { return lsn_; }

    inline void setLSN(lsn_t lsn) { this->lsn_ = lsn; }

    inline txn_id_t getTxnId() { return txn_id_; }

    inline lsn_t getPrevLSN() { return prev_lsn_; }

    inline LogRecordType& getLogRecordType() { return log_record_type_; }

private:
    int32_t size_{ 0 }; // ¼ÇÂ¼³¤¶È
    lsn_t lsn_{ INVALID_LSN };
    txn_id_t txn_id_{ INVALID_TXN_ID };
    lsn_t prev_lsn_{ INVALID_LSN };
    LogRecordType log_record_type_{ LogRecordType::INVALID };

    // case1: for delete operation
    RID delete_rid_;
    Tuple delete_tuple_;

    // case3: for insert operation
    RID insert_rid_;
    Tuple insert_tuple_;

    // case2: for update operation
    RID update_rid_;
    Tuple old_tuple_;
    Tuple new_tuple_;

    string relname_;

    static const int HEADER_SIZE = sizeof(int32_t) + 2 * sizeof(lsn_t) + sizeof(txn_id_t) + sizeof(LogRecordType);
};