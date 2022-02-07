#include"Subsystem1_Manager.h"

int test() {

	Subsystem1_Manager::mgr.UserName = "User1";
	Subsystem1_Manager::mgr.DBName = "DB1";

	//创建表
	Attr_Info attr_info;
	strcpy(attr_info.DBName, "DB1"); strcpy(attr_info.Rel_Name, "Rel2"); strcpy(attr_info.Creator, "User1");
	strcpy(attr_info.Attr_Name, "id");                                   attr_info.Num_of_Change_Records = 0;
	attr_info.type = AttrType::INT;  attr_info.Offset = 0;               attr_info.Length = 4;


	Attr_Info attr_info2;
	strcpy(attr_info2.DBName, "DB1"); strcpy(attr_info2.Rel_Name, "Rel2"); strcpy(attr_info2.Creator, "User1");
	strcpy(attr_info2.Attr_Name, "price");                                   attr_info2.Num_of_Change_Records = 0;
	attr_info2.type = AttrType::FLOAT;  attr_info2.Offset = 4;               attr_info2.Length = 4;


	Attr_Info attr_info3;
	strcpy(attr_info3.DBName, "DB1"); strcpy(attr_info3.Rel_Name, "Rel2"); strcpy(attr_info3.Creator, "User1");
	strcpy(attr_info3.Attr_Name, "name");                                   attr_info3.Num_of_Change_Records = 0;
	attr_info3.type = AttrType::STRING;  attr_info3.Offset = 8;               attr_info3.Length = 20;

	vector<Attr_Info> attr_list;  attr_list.push_back(attr_info);  attr_list.push_back(attr_info2); attr_list.push_back(attr_info3);
	Subsystem1_Manager::mgr.Create_Rel("Rel2", attr_list);



	//查询表的属性
	/*cout << "------------------------------" << endl;
	vector<Attr_Info> vec = Subsystem1_Manager::mgr.lookup_Attrs("Rel1");
	for (Attr_Info attr_info : vec) {
		cout << attr_info.Attr_Name << " " << attr_info.Length << " " << attr_info.Offset << endl;
	}*/




	//增加记录
	/*int id = 1;          int* id_p = &id;          char* id_ch = (char*)(id_p);
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
	RID rid_ = Subsystem1_Manager::mgr.Insert_Reocrd( "Rel1", record);
	cout << rid_.blockID << " " << rid_.slotID << endl;*/

	//增加第2条记录
	/*int id = 2;          int* id_p = &id;          char* id_ch = (char*)(id_p);
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
	RID rid_ = Subsystem1_Manager::mgr.Insert_Reocrd( "Rel1", record);
	cout << rid_.blockID << " " << rid_.slotID << endl;*/




	//获取表的所有rid    并修改第一个记录的id =110
	/*cout << "------------------------------" << endl;
	vector<RID> rid_vec = Subsystem1_Manager::mgr.Scan_Record("Rel1");
	for (RID rid : rid_vec) {
		cout << rid.blockID << " " << rid.slotID << endl;
	}

	Attr_Info attr__; strcpy(attr__.Attr_Name, "id"); attr__.Length = 4; attr__.Offset = 0;
	int new_id = 110; int* new_id_p = &new_id; char* new_id_ch = (char*)new_id_p;
	Subsystem1_Manager::mgr.Update_Record("Rel1", rid_vec[0], attr__, new_id_ch);*/








	//获取表信息
	/*cout << "------------------------------" << endl;
	Rel_Info rel;
	Subsystem1_Manager::mgr.lookup_Rel("Rel1", rel);
	cout << rel.DBName << "  Record_num:: " << rel.Record_Num << " " << endl;*/









	









    //扫描所有关系
    cout << "-----------------------------------" << endl;
    Subsystem1_Manager::mgr.Scan_rel();
    //扫描所有属性
	cout << "-----------------------------------" << endl;
	Subsystem1_Manager::mgr.Scan_attribute();
	Subsystem1_Manager::mgr.Scan_Record("Rel1");

	PF_BufferMgr::pf_buffermgr.Write_all_buffer();
	cout << "buffer writed" << endl;
	Page_Mgr::page_mgr.Write_page();
	cout << "page writed" << endl;
	return 0;
}