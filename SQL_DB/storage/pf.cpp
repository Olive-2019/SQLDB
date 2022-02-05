/*



*/
#include"pf.h"

Page_Mgr Page_Mgr::page_mgr;
PF_BufferMgr PF_BufferMgr::pf_buffermgr;
Subsystem1_Manager Subsystem1_Manager::mgr;



int pf_example() {

	
	//插入记录
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
	RID rid_ = mgr.Insert_Reocrd("User1", "DB1", "Rel1", record);
	cout << rid_.blockID << " " << rid_.slotID << endl;*/

	vector<RID> rid_vec = Subsystem1_Manager::mgr.Scan_Record("Rel1");
	for (RID rid : rid_vec) {
		cout << rid.blockID << " " << rid.slotID << endl;
	}




	//创建表
	/*Rel_Info rel_info;
	strcpy(rel_info.DBName, "DB1"); strcpy(rel_info.Rel_Name, "Rel1"); strcpy(rel_info.Creator, "User1");
	rel_info.Record_Num = 0;
	Rel_Info rel_info2;
	strcpy(rel_info2.DBName, "DB2"); strcpy(rel_info2.Rel_Name, "Rel1"); strcpy(rel_info2.Creator, "User1");
	rel_info2.Record_Num = 0;
	Rel_Info rel_info3;
	strcpy(rel_info3.DBName, "DB1"); strcpy(rel_info3.Rel_Name, "Rel2"); strcpy(rel_info3.Creator, "User1");
	rel_info3.Record_Num = 0;
	mgr.Add_rel(rel_info);
	mgr.Add_rel(rel_info2);
	mgr.Add_rel(rel_info3);
	cout << "--------------------" << endl;*/

	//删除表
	/*mgr.Delete_Rel("User1", "DB1", "Rel1");
	*/

	//遍历当前所有表
	//mgr.Scan_rel();


	//增加属性  、 遍历所有属性
	/*Attr_Info attr_info;
	strcpy(attr_info.DBName, "DB1"); strcpy(attr_info.Rel_Name, "Rel1"); strcpy(attr_info.Creator, "User1");
	strcpy(attr_info.Attr_Name, "id");                                   attr_info.Num_of_Change_Records = 0;
	attr_info.type = AttrType::INT;  attr_info.Offset = 0;               attr_info.Length = 4;
	attr_info.distribution.type = Distribution_Type::NORMAL; attr_info.distribution.nor_eve.nor.mu = 0; attr_info.distribution.nor_eve.nor.sigma = 0;

	Attr_Info attr_info2;
	strcpy(attr_info2.DBName, "DB1"); strcpy(attr_info2.Rel_Name, "Rel1"); strcpy(attr_info2.Creator, "User1");
	strcpy(attr_info2.Attr_Name, "price");                                   attr_info2.Num_of_Change_Records = 0;
	attr_info2.type = AttrType::FLOAT;  attr_info2.Offset = 4;               attr_info2.Length = 4;
	attr_info2.distribution.type = Distribution_Type::NORMAL; attr_info2.distribution.nor_eve.nor.mu = 0; attr_info2.distribution.nor_eve.nor.sigma = 0;

	Attr_Info attr_info3;
	strcpy(attr_info3.DBName, "DB1"); strcpy(attr_info3.Rel_Name, "Rel1"); strcpy(attr_info3.Creator, "User1");
	strcpy(attr_info3.Attr_Name, "name");                                   attr_info3.Num_of_Change_Records = 0;
	attr_info3.type = AttrType::STRING;  attr_info3.Offset = 8;               attr_info3.Length = 20;
	attr_info3.distribution.type = Distribution_Type::NORMAL; attr_info3.distribution.nor_eve.nor.mu = 0; attr_info3.distribution.nor_eve.nor.sigma = 0;

	vector<Attr_Info> attr_list;  attr_list.push_back(attr_info);  attr_list.push_back(attr_info2); attr_list.push_back(attr_info3);
	mgr.Add_attribute(attr_list);
	cout << "--------------------" << endl;
	mgr.Scan_attribute();*/


	//获取表属性
	/*Attr_Info attr_info;
	mgr.lookup_Attr("User1", "DB1", "Rel1", "id", attr_info);
	cout << attr_info.Attr_Name << " " << attr_info.type << " " << attr_info.Length << endl;*/

	//获取表属性
	/*vector<Attr_Info> vec = mgr.lookup_Attrs("User1", "DB1", "Rel1");
	for (Attr_Info attr_info : vec) {
		cout << attr_info.Attr_Name << " " << attr_info.type << " " << attr_info.Length << endl;
	}*/

	//获取表信息
	/*Rel_Info rel_info;
	mgr.lookup_Rel("User1", "DB1", "Rel1", rel_info);
	cout << rel_info.Rel_Name << " " << rel_info.PageID_Head << endl;*/


	PF_BufferMgr::pf_buffermgr.Write_all_buffer();
	Page_Mgr::page_mgr.Write_page();

	return 0;
}