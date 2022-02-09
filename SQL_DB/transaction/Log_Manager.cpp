#include "Log_Manager.h"


void LogManager::runFlushThread() {
    // 不断写入磁盘
    this->flush_thread_ = new std::thread([this]() {
        while (true) {
            std::unique_lock<std::mutex> lock(this->latch_);
            //原代码:this->cv_.wait_for(lock, log_timeout, [this] { return this->force_flush_flag_; });
            this->cv_.wait(lock, [this] { return this->force_flush_flag_; });
            this->force_flush_flag_ = false;
            if (this->stop_flush_thread_) {
                this->stop_flush_thread_ = false;
                return;
            }
            // if the LogBuffer is full || force flush || timeout -> flush
            std::swap(this->log_buffer_, this->flush_buffer_);
            //原代码:this->disk_manager_->WriteLog(flush_buffer_, strlen(flush_buffer_));
            writeLog(flush_buffer_, strlen(flush_buffer_));
            this->offset_ = 0;
            this->setPersistentLSN(this->getNextLSN());
            lock.unlock();
        }
        });
}

void LogManager::stopFlushThread() {
    this->stop_flush_thread_ = true;
    this->flush_thread_->join();
}

void LogManager::forceFlush() {
    std::lock_guard<std::mutex> lock(this->latch_);
    this->force_flush_flag_ = true;
    this->cv_.notify_one();
}

lsn_t LogManager::appendLogRecord(LogRecord* log_record) {
    if (log_record->getLogRecordType() == LogRecordType::INVALID) return INVALID_LSN;
    std::unique_lock<std::mutex> lock(this->latch_);
    log_record->setLSN(this->next_lsn_++);

    // 记录对应的offset_;
    lsn_mapping_[log_record->getLSN()] = this->offset_;
    
    // serialize header of log_record
    memcpy(log_buffer_ + offset_, log_record, LogRecord::HEADER_SIZE);
    // serialize data of log_record
    this->offset_ += LogRecord::HEADER_SIZE;
    // serialize INSERT LogRecord
    if (log_record->getLogRecordType() == LogRecordType::INSERT) {
        memcpy(log_buffer_ + this->offset_, &(log_record->getInsertRID()), sizeof(RID));
        this->offset_ += sizeof(RID);
        log_record->getInsertTuple().serializeTo(log_buffer_ + this->offset_);
        this->offset_ += sizeof(int32_t) + log_record->getInsertTuple().getLength();
    }
    // serialize DELETE LogRecord
    if (log_record->getLogRecordType() == LogRecordType::DELETE) {
        memcpy(log_buffer_ + this->offset_, &(log_record->getDeleteRID()), sizeof(RID));
        this->offset_ += sizeof(RID);
        log_record->getDeleteTuple().serializeTo(log_buffer_ + this->offset_);
        this->offset_ += sizeof(int32_t) + log_record->getDeleteTuple().getLength();
    }
    // serialize UPDATE LogRecord
    if (log_record->getLogRecordType() == LogRecordType::UPDATE) {
        memcpy(log_buffer_ + this->offset_, &(log_record->getUpdateRID()), sizeof(RID));
        this->offset_ += sizeof(RID);
        log_record->getOriginalTuple().serializeTo(log_buffer_ + this->offset_);
        this->offset_ += sizeof(int32_t) + log_record->getOriginalTuple().getLength();
        log_record->getUpdateTuple().serializeTo(log_buffer_ + this->offset_);
        this->offset_ += sizeof(int32_t) + log_record->getUpdateTuple().getLength();
    }
    // otherwise, there is no need to serialize anymore (besides the header) -> return

    if (log_record->getLogRecordType() == LogRecordType::INSERT ||
        log_record->getLogRecordType() == LogRecordType::DELETE ||
        log_record->getLogRecordType() == LogRecordType::UPDATE) {
        memcpy(log_buffer_ + this->offset_, log_record->relname_.c_str(), log_record->relname_.length());
        this->offset_ += log_record->relname_.length();
    }

    return log_record->getLSN();
}

int32_t LogManager::deserializeLogRecord(const char* data, LogRecord& log_record) {
    int32_t offset = 0;
    log_record.size_ = *(int32_t*)(data);
    offset += sizeof(int32_t);
    log_record.lsn_ = *(lsn_t*)(data + offset);
    offset += sizeof(lsn_t);
    log_record.txn_id_ = *(txn_id_t*)(data + offset);
    offset += sizeof(txn_id_t);
    log_record.prev_lsn_ = *(lsn_t*)(data + offset);
    offset += sizeof(lsn_t);
    
    auto log_type = log_record.log_record_type_ = *(LogRecordType*)(data + offset);

    offset += sizeof(LogRecordType);

    assert(offset == LogRecord::HEADER_SIZE);

    if (log_type == LogRecordType::INSERT) {
        memcpy(&log_record.insert_rid_, data + offset, sizeof(RID));
        offset += sizeof(RID);
        offset += log_record.insert_tuple_.deserializeFrom(data + offset);
    }
    else if (log_type == LogRecordType::DELETE) {
        memcpy(&log_record.delete_rid_, data + offset, sizeof(RID));
        offset += sizeof(RID);
        offset += log_record.delete_tuple_.deserializeFrom(data + offset);
    }
    else if (log_type == LogRecordType::UPDATE) {
        memcpy(&log_record.update_rid_, data + offset, sizeof(RID));
        offset += sizeof(RID);
        offset += log_record.old_tuple_.deserializeFrom(data + offset);
        offset += log_record.new_tuple_.deserializeFrom(data + offset);
    }

    if (log_type == LogRecordType::INSERT || log_type == LogRecordType::DELETE || log_type == LogRecordType::UPDATE) {
        char* relname = new char[log_record.size_ - offset];
        memcpy(relname, data + offset, log_record.size_ - offset);
        log_record.relname_ = string(relname);
        delete[] relname;
    }

    return log_record.size_;
}

void LogManager::writeLog(char* log_data, int size) {
    if (size == 0) {
        return;
    }

    log_io_.write(log_data, size);
    if (log_io_.bad()) {
        cerr << ("I/O error while writing log\n");
        return;
    }
    // needs to flush to keep disk file in sync
    log_io_.flush();

}

bool LogManager::readLog(char* log_data, int size, int offset) {
    log_io_.seekp(offset);
    log_io_.read(log_data, size);

    if (log_io_.bad()) {
        cerr << ("I/O error while reading log\n");
        return false;
    }

    int read_count = log_io_.gcount();
    if (read_count < size) {
        log_io_.clear();
        memset(log_data + read_count, 0, size - read_count);
    }

    return true;
}

bool LogManager::Do(const LogRecord& log_record) {
    return false;
}