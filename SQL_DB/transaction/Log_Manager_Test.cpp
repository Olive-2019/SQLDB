#include <cstdio>
#include "Log_Manager.h"
#include "Log_Record.h"
#include "Transaction_Manager.h"

#include <direct.h>

using namespace std;

class LogManagerTest {
public:
    // 缓存写入和读出的测试
    void ReadWriteTest() {
        remove("sys/Log/test.log");
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
        vector<Value> vs;
        Value v1, v2, v3;
        int a = 10;
        float b = 20.0;
        char c[20];
        memset(c, 0, sizeof(c));
        memcpy(c, "testtest", sizeof("testtest"));
        v1.type = INT;
        v2.type = FLOAT;
        v3.type = STRING;
        v1.data = &a;
        v2.data = &b;
        v3.data = c;
        vs.push_back(v1);
        vs.push_back(v2);
        vs.push_back(v3);

        Tuple tuple("R2", vs, rid);
        printf("rel = %s\n", tuple.getRel().c_str());

        txn_id_t tid = 1;
        lsn_t plsn = 2;
        // insert
        LogRecord inst(tid, plsn, LogRecordType::INSERT, rid, tuple, "R2");
        log_mgr.appendLogRecord(&inst);
        log_mgr.deserializeLogRecord(log_mgr.log_buffer_ + 20, rs);
        printf("inst.getSize() == rs.getSize():%d\n", inst.getSize() == rs.getSize());
        // delete
        LogRecord delt(tid, plsn, LogRecordType::DELETE, rid, tuple, "R2");
        log_mgr.appendLogRecord(&delt);
        log_mgr.deserializeLogRecord(log_mgr.log_buffer_ + 20 + inst.getSize(), rs);
        printf("delt.getSize() == rs.getSize():%d\n", delt.getSize() == rs.getSize());
        // update
        LogRecord updt(tid, plsn, LogRecordType::UPDATE, rid, tuple, tuple, "R2");
        log_mgr.appendLogRecord(&updt);
        log_mgr.deserializeLogRecord(log_mgr.log_buffer_ + 20 + inst.getSize() + delt.getSize(), rs);
        printf("updt.getSize() == rs.getSize():%d\n", updt.getSize() == rs.getSize());
    }

    // 外存日志读写
    void WR2StorageTest() {
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
    void FlushTest() {
        // TODO 线程操作
        remove("sys/Log/test.log");
        LogManager log_mgr("sys/Log/test");
        log_mgr.runFlushThread();

        LockManager lock_mgr;
        TransactionManager ts_mgr{ &lock_mgr, &log_mgr };
        Transaction* txn = ts_mgr.begin();
        log_mgr.forceFlush();

        while (true) {
            char x;
            cin >> x;
            if (x == 'q') break;
        }
        log_mgr.stopFlushThread();
    }

    void RedoTest() {
        remove("sys/Log/test.log");
        LogManager log_mgr("sys/Log/test");
        log_mgr.runFlushThread();

        LockManager lock_mgr;
        TransactionManager ts_mgr{ &lock_mgr, &log_mgr };

        Transaction* txn = ts_mgr.begin();

        log_mgr.forceFlush();

        vector<Value> vs;
        Value v1, v2, v3;
        int a = 3210;
        float b = 440.0;
        char c[20];
        memset(c, 0, sizeof(c));
        memcpy(c, "advvvv", sizeof("advvvv"));
        v1.type = INT;
        v2.type = FLOAT;
        v3.type = STRING;
        v1.data = &a;
        v2.data = &b;
        v3.data = c;
        vs.push_back(v1);
        vs.push_back(v2);
        vs.push_back(v3);

        TableWriteRecord twr{ RID{}, WType::INSERT, "R2", vs };

        ts_mgr.write(twr, txn);
        ts_mgr.commit(txn);

        while (true) {
            char x;
            cin >> x;
            if (x == 'q') break;
        }

        log_mgr.stopFlushThread();

        log_mgr.redo();
    }

    void RunTest() {
        //ReadWriteTest(); // OK
        //WR2StorageTest(); // OK
        //FlushTest(); // OK
        //RedoTest(); // OK
    }
};

//int main(void) {
//    Subsystem1_Manager::mgr.UserName = "User1";
//    Subsystem1_Manager::mgr.DBName = "DB1";
//
//    auto rids = Subsystem1_Manager::mgr.Scan_Record("R2");
//    for (auto rid : rids) {
//        char* data = Subsystem1_Manager::mgr.Find_Record_by_RID("R2", rid);
//        auto values = TableWriter::GetValues("R2", data);
//        for (const auto& value : values) {
//            if (value.type == INT) {
//                printf("%d", *(int*)value.data);
//            }
//            else if (value.type == FLOAT) {
//                printf("%f", *(float*)value.data);
//            }
//            else if (value.type == STRING) {
//                printf("%s", (char*)value.data);
//            }
//            printf(" ");
//        }
//        printf("RID:(%d,%d)", rid.blockID, rid.slotID);
//        puts("");
//    }
//
//    LogManagerTest test;
//    test.RunTest();
//
//    rids = Subsystem1_Manager::mgr.Scan_Record("R2");
//    for (auto rid : rids) {
//        char* data = Subsystem1_Manager::mgr.Find_Record_by_RID("R2", rid);
//        auto values = TableWriter::GetValues("R2", data);
//        for (const auto& value : values) {
//            if (value.type == INT) {
//                printf("%d", *(int*)value.data);
//            }
//            else if (value.type == FLOAT) {
//                printf("%f", *(float*)value.data);
//            }
//            else if (value.type == STRING) {
//                printf("%s", (char*)value.data);
//            }
//            printf(" ");
//        }
//        printf("RID:(%d,%d)", rid.blockID, rid.slotID);
//        puts("");
//    }
//}