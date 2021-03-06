#pragma once

#include "../global.h"
#include "Log_Record.h"
#include "Transaction.h"

#include <condition_variable>
#include <chrono>
#include <fstream>

static constexpr int LOG_BUFFER_SIZE = (BUFFER_NUM + 1) * 4096; // 4096 == BlockSize

class LogManager {
    friend class LogManagerTest;
public:
    /**
     *   ios::app：　　　 //以追加的方式打开文件
     *   ios::binary：　  //以二进制方式打开文件，缺省的方式是文本方式。
     *   ios::in：　　　  //文件以输入方式打开（文件数据输入到内存） 
     *   ios::out：　　　 //文件以输出方式打开（内存数据输出到文件）
     *   ios::trunc：　   //如果文件存在，把文件长度设为0
     */

    explicit LogManager(const std::string& dbname)
        : offset_(0), stop_flush_thread_(false), next_lsn_(0), persistent_lsn_(INVALID_LSN) {
        log_buffer_ = new char[LOG_BUFFER_SIZE];
        flush_buffer_ = new char[LOG_BUFFER_SIZE];

        log_name_ = dbname + ".log";
        log_io_.open(log_name_, std::ios::binary | std::ios::in | std::ios::app | std::ios::out);
        if (!log_io_.is_open()) {
            log_io_.clear();
            // create a new file
            log_io_.open(log_name_, std::ios::binary | std::ios::trunc | std::ios::app | std::ios::out);
            log_io_.close();
            // reopen with original mode
            log_io_.open(log_name_, std::ios::binary | std::ios::in | std::ios::app | std::ios::out);
            if (!log_io_.is_open()) {
                cerr << "log open failed" << endl;
            }
        }
    }

    ~LogManager() {
        delete[] log_buffer_;
        delete[] flush_buffer_;
        log_buffer_ = nullptr;
        flush_buffer_ = nullptr;

        log_io_.close();
    }

    // 用于将当前日志缓存写入磁盘
    void runFlushThread();
    void stopFlushThread();
    void forceFlush();

    // 缓存添加日志记录
    lsn_t appendLogRecord(LogRecord* log_record);

    // 从日志缓存中读取一条记录
    int32_t deserializeLogRecord(const char* data, LogRecord& log_record);

    inline lsn_t getNextLSN() { return next_lsn_; }
    inline lsn_t getPersistentLSN() { return persistent_lsn_; }
    inline void setPersistentLSN(lsn_t lsn) { persistent_lsn_ = lsn; }
    inline char* getLogBuffer() { return log_buffer_; }

    void redo();
    void undo();

    // 本应更底层来做
    void writeLog(char* log_data, int size);
    bool readLog(char* log_data, int size, int offset);

private:
    size_t offset_;
    size_t goffset_; // 读取磁盘文件时使用

    std::atomic_bool stop_flush_thread_;
    bool force_flush_flag_;

    std::atomic<lsn_t> next_lsn_;
    /** The log records before and including the persistent lsn have been written to disk. */
    std::atomic<lsn_t> persistent_lsn_;

    char* log_buffer_;
    char* flush_buffer_;

    std::mutex latch_;

    std::thread* flush_thread_;

    std::condition_variable cv_;

    // 活动事务的最后一个LSN
    std::map<txn_id_t, lsn_t> active_txn_;
    // LSN对应的global_offset和log_record_size
    std::map<lsn_t, pair<size_t, int32_t>> lsn_mapping_;

    // 和磁盘交互
    std::fstream log_io_;
    std::string log_name_;
};