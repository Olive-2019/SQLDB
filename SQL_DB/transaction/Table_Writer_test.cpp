#include "Table_Writer.h"
#include "../storage/Subsystem1_Manager.h"

using namespace std;

void Init() {
    Attr_Info attr_info;
	strcpy(attr_info.DBName, "DB1"); 
    strcpy(attr_info.Rel_Name, "R1"); 
    strcpy(attr_info.Creator, "User1");
	strcpy(attr_info.Attr_Name, "id");  
    attr_info.Num_of_Change_Records = 0;
	attr_info.type = AttrType::INT;  
    attr_info.Offset = 0;               
    attr_info.Length = 4;

    Attr_Info attr_info2;
    strcpy(attr_info2.DBName, "DB1"); 
    strcpy(attr_info2.Rel_Name, "R1"); 
    strcpy(attr_info2.Creator, "User1");
    strcpy(attr_info2.Attr_Name, "price");                                   
    attr_info2.Num_of_Change_Records = 0;
    attr_info2.type = AttrType::FLOAT;  
    attr_info2.Offset = 4;               
    attr_info2.Length = 4;

    Attr_Info attr_info3;
	strcpy(attr_info3.DBName, "DB1"); 
    strcpy(attr_info3.Rel_Name, "R1"); 
    strcpy(attr_info3.Creator, "User1");
	strcpy(attr_info3.Attr_Name, "name");                                   
    attr_info3.Num_of_Change_Records = 0;
	attr_info3.type = AttrType::STRING;  
    attr_info3.Offset = 8;    
    attr_info3.Length = 20;

    vector<Attr_Info> attr_list;  
    attr_list.push_back(attr_info);  
    attr_list.push_back(attr_info2); 
    attr_list.push_back(attr_info3);
   
    Subsystem1_Manager::mgr.Create_Rel("R1", attr_list);
}

vector<Value> GetValues(string rel, char* data) {
    auto attrs = Subsystem1_Manager::mgr.lookup_Attrs(rel);
    int32_t offset = 0;
    vector<Value> values;
    for (const auto& attr : attrs) {
        Value value;
        value.type = attr.type;
        value.data = data + offset;
        //memcpy(value.data, data + offset, attr.Length);
        values.push_back(value);
        offset += attr.Length;
    }
    return values;
}

void InsertTest() {
    LockManager lock_mgr;
    LogManager log_mgr("sys/Log/test");
    TableWriter tw;
    Transaction txn(1);

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
    Tuple tuple("Rel1", vs, RID{});
    
    tw.insertTuple(tuple, &txn, &lock_mgr, &log_mgr);
    auto rids = Subsystem1_Manager::mgr.Scan_Record("Rel1");
    for (auto rid : rids) {
        char* data = Subsystem1_Manager::mgr.Find_Record_by_RID("Rel1", rid);
        auto values = GetValues("Rel1", data);
        for (const auto& value : values) {
            if (value.type == INT) {
                printf("%d", *(int*)value.data);
            }
            else if (value.type == FLOAT) {
                printf("%f", *(float*)value.data);
            }
            else if (value.type == STRING) {
                printf("%s", (char*)value.data);
            }
            printf(" ");
        }
        puts("");
    }
}

void DeleteTest() {
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
    Tuple tuple("Rel1", vs, RID{});

    LockManager lock_mgr;
    LogManager log_mgr("sys/Log/test");
    TableWriter tw;
    Transaction txn(1);

    //tw.insertTuple(tuple, &txn, &lock_mgr, &log_mgr);

    auto rids = Subsystem1_Manager::mgr.Scan_Record("Rel1");
    for (auto rid : rids) {
        char* data = Subsystem1_Manager::mgr.Find_Record_by_RID("Rel1", rid);
        auto values = GetValues("Rel1", data);
        for (const auto& value : values) {
            if (value.type == INT) {
                printf("%d", *(int*)value.data);
            }
            else if (value.type == FLOAT) {
                printf("%f", *(float*)value.data);
            }
            else if (value.type == STRING) {
                printf("%s", (char*)value.data);
            }
            printf(" ");
        }
        printf("RID:(%d,%d)", rid.blockID, rid.slotID);
        puts("");
    }

    printf("back::RID:(%d,%d)\n", rids.back().blockID, rids.back().slotID);
    Tuple tuple2("Rel1", vs, rids.back());
    printf("delete : %d\n", tw.deleteTuple(tuple2, &txn, &lock_mgr, &log_mgr));

    rids = Subsystem1_Manager::mgr.Scan_Record("Rel1");
    for (auto rid : rids) {
        char* data = Subsystem1_Manager::mgr.Find_Record_by_RID("Rel1", rid);
        auto values = GetValues("Rel1", data);
        for (const auto& value : values) {
            if (value.type == INT) {
                printf("%d", *(int*)value.data);
            }
            else if (value.type == FLOAT) {
                printf("%f", *(float*)value.data);
            }
            else if (value.type == STRING) {
                printf("%s", (char*)value.data);
            }
            printf(" ");
        }
        printf("RID:(%d,%d)", rid.blockID, rid.slotID);
        puts("");
    }
}

void UpdateTest() {
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

    LockManager lock_mgr;
    LogManager log_mgr("sys/Log/test");
    TableWriter tw;
    Transaction txn(1);

    auto rids = Subsystem1_Manager::mgr.Scan_Record("Rel1");
    for (auto rid : rids) {
        char* data = Subsystem1_Manager::mgr.Find_Record_by_RID("Rel1", rid);
        auto values = GetValues("Rel1", data);
        for (const auto& value : values) {
            if (value.type == INT) {
                printf("%d", *(int*)value.data);
            }
            else if (value.type == FLOAT) {
                printf("%f", *(float*)value.data);
            }
            else if (value.type == STRING) {
                printf("%s", (char*)value.data);
            }
            printf(" ");
        }
        printf("RID:(%d,%d)", rid.blockID, rid.slotID);
        puts("");
    }

    Tuple old("Rel1", vs, rids.back());
    vector<Value> vs2(vs);
    int na = 66;
    float nb = 77.4;
    char nc[20];
    memcpy(nc, "galaal", sizeof("galaal"));
    vs2[0].data = &na;
    vs2[1].data = &nb;
    vs2[2].data = nc;
    Tuple upd("Rel1", vs2, rids.back());

    tw.updateTuple(old, upd, &txn, &lock_mgr, &log_mgr);

    rids = Subsystem1_Manager::mgr.Scan_Record("Rel1");
    for (auto rid : rids) {
        char* data = Subsystem1_Manager::mgr.Find_Record_by_RID("Rel1", rid);
        auto values = GetValues("Rel1", data);
        for (const auto& value : values) {
            if (value.type == INT) {
                printf("%d", *(int*)value.data);
            }
            else if (value.type == FLOAT) {
                printf("%f", *(float*)value.data);
            }
            else if (value.type == STRING) {
                printf("%s", (char*)value.data);
            }
            printf(" ");
        }
        printf("RID:(%d,%d)", rid.blockID, rid.slotID);
        puts("");
    }
}
//
//int main(void) {
//    Subsystem1_Manager::mgr.UserName = "User1";
//    Subsystem1_Manager::mgr.DBName = "DB1";
//
//    //InsertTest(); // OK
//    //DeleteTest(); // OK
//    //UpdateTest(); // OK
//
//    //auto rids = Subsystem1_Manager::mgr.Scan_Record("Rel1");
//    //for (auto rid : rids) {
//    //    printf("RID:(%d,%d)", rid.blockID, rid.slotID);
//    //    puts("");
//    //}
//    //Subsystem1_Manager::mgr.Delete_Record("Rel1", rids);
//
//    return 0;
//}