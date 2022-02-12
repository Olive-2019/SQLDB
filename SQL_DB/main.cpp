#include "Subsystem1.h"
#include "global.h"
#include "parser/parser.h"

void load(string filePath = "data.txt") {
	FILE* file = fopen(filePath.c_str(), "r");
	while (true) {
		cout << "here" << endl;
		int id;
		float price;
		char temp[100];
		int is_EOF=fscanf(file, "%d", &id);
		if (is_EOF == EOF) {
			return;
		}
		fscanf(file, "%f", &price);
		fscanf(file, "%s", temp);
		char* record = new char[100];
		char* t = record;
		memcpy(record, &id, sizeof(int));
		record += sizeof(int);
		memcpy(record, &price, sizeof(float));
		record += sizeof(float);
		memcpy(record, temp, strlen(temp) + 1);
		Subsystem1_Manager::mgr.Insert_Reocrd("R1", t);
	}
}
/*
int main() {
	Subsystem1_Manager::mgr.UserName = "User1";
	Subsystem1_Manager::mgr.DBName = "DB1";

	auto vec = Subsystem1_Manager::mgr.Scan_Record("R1");

	Subsystem1_Manager::mgr.Scan_rel();
	Subsystem1_Manager::mgr.Scan_attribute();
	Subsystem1_Manager::mgr.UserName = "User1";
	Subsystem1_Manager::mgr.DBName = "DB1";
	//load();
	RBparse();

    PF_BufferMgr::pf_buffermgr.Write_all_buffer();
	Page_Mgr::page_mgr.Write_page();

	//while (true);
	return 0;
}*/