#include <cstdio>
#include "Log_Manager.h"
#include "Log_Record.h"

#include <direct.h>

using namespace std;

class LogManagerTest {
public:
    // 缓存写入和读出的测试
    void read_write_test() {
        //char cwd[256];
        //getcwd(cwd, 256);
        //cout << cwd << endl;

        LogManager log_mgr("sys/Log/test");

        txn_id_t txn_id = 0;
        lsn_t prev_lsn = -1;
        LogRecordType log_record_type = LogRecordType::BEGIN;

        LogRecord log_record(txn_id, prev_lsn, log_record_type);

        printf("0 == (int)log_mgr.offset_:%d\n", 0 == (int)log_mgr.offset_);
        log_mgr.appendLogRecord(&log_record);
        printf("20 == (int)log_mgr.offset_:%d\n", 20 == (int)log_mgr.offset_);

        LogRecord rs;
        log_mgr.deserializeLogRecord(log_mgr.log_buffer_, rs);
        printf("20 == rs.getSize():%d\n", 20 == rs.getSize());
        printf("0 == rs.getTxnId():%d\n", 0 == rs.getTxnId());
        printf("-1 == rs.getPrevLSN():%d\n", -1 == rs.getPrevLSN());
        printf("LogRecordType::BEGIN == rs.getLogRecordType():%d\n", LogRecordType::BEGIN == rs.getLogRecordType());
 
        RID rid{ 1, 2 };
        string rel = "test";
        vector<Value> values;
        Value v;
        int a = 3;
        v.type = INT;
        v.data = &a;
        values.push_back(v);
        Tuple tuple(rel, values, rid);
        printf("rel = %s\n", tuple.getRel().c_str());

        txn_id_t tid = 1;
        lsn_t plsn = 2;
        // insert
        LogRecord inst(tid, plsn, LogRecordType::INSERT, rid, tuple, rel);
        log_mgr.appendLogRecord(&inst);
        log_mgr.deserializeLogRecord(log_mgr.log_buffer_ + 20, rs);
        printf("inst.getSize() == rs.getSize():%d\n", inst.getSize() == rs.getSize());
        // delete
        LogRecord delt(tid, plsn, LogRecordType::DELETE, rid, tuple, rel);
        log_mgr.appendLogRecord(&delt);
        log_mgr.deserializeLogRecord(log_mgr.log_buffer_ + 20 + inst.getSize(), rs);
        printf("delt.getSize() == rs.getSize():%d\n", delt.getSize() == rs.getSize());
        //// update
        LogRecord updt(tid, plsn, LogRecordType::UPDATE, rid, tuple, tuple, rel);
        log_mgr.appendLogRecord(&updt);
        log_mgr.deserializeLogRecord(log_mgr.log_buffer_ + 20 + inst.getSize() + delt.getSize(), rs);
        printf("updt.getSize() == rs.getSize():%d\n", updt.getSize() == rs.getSize());
    }

    // 外存日志读写
    void wr2storage_test() {
        LogManager log_mgr("sys/Log/test");
        txn_id_t txn_id = 0;
        lsn_t prev_lsn = -1;
        LogRecordType log_record_type = LogRecordType::BEGIN;
        LogRecord log_record(txn_id, prev_lsn, log_record_type);
        log_mgr.appendLogRecord(&log_record);


        // 写入20字节
        log_mgr.writeLog(log_mgr.log_buffer_, log_mgr.offset_); // 注意是追加的方式写入

        // 读出20字节, 应该从buffer+20处开始写入
        log_mgr.readLog(log_mgr.log_buffer_ + log_mgr.offset_, log_mgr.offset_, 0);
        
        LogRecord rs;
        log_mgr.deserializeLogRecord(log_mgr.log_buffer_ + log_mgr.offset_, rs);
        printf("20 == rs.getSize():%d\n", 20 == rs.getSize());
        printf("0 == rs.getTxnId():%d\n", 0 == rs.getTxnId());
        printf("-1 == rs.getPrevLSN():%d\n", -1 == rs.getPrevLSN());
        printf("LogRecordType::BEGIN == rs.getLogRecordType():%d\n", LogRecordType::BEGIN == rs.getLogRecordType());
    }

    // flush
    void flush_test() {
        // TODO 线程操作
    }

    void redo_test() {
        LogManager log_mgr("sys/Log/test");
    }

    void run() {
        //read_write_test(); // OK
        //wr2storage_test(); // OK
        //flush_test();
    }
};

int main(void) {
    LogManagerTest test;
    //test.run();
}