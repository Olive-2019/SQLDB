#include "Subsystem1.h"
#include "global.h"
#include "parser/parser.h"

int main() {

	Subsystem1_Manager::mgr.Scan_rel();


	int id = 2;          int* id_p = &id;          char* id_ch = (char*)(id_p);
	float price =32.14;  float* price_p = &price;  char* price_ch = (char*)(price_p);
	char name[MAX_NAME_LENGTH] = "XiMing";
	char record[28];
	for (int i = 0; i < 4; i++) {
		record[i] = id_ch[i];
	}
	for (int i = 0; i < 4; i++) {
		record[i + 4] = price_ch[i];
	}
	for (int i = 0; i < 20; i++) {
		record[i + 8] = name[i];
	}
	RID rid_ = Subsystem1_Manager::mgr.Insert_Reocrd("Rel2", record);

	Subsystem1_Manager::mgr.Scan_rel();

	RBparse();
}