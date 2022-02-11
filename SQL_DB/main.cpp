#include "Subsystem1.h"
#include "global.h"
#include "parser/parser.h"

int main() {
	Subsystem1_Manager::mgr.UserName = "User1";
	Subsystem1_Manager::mgr.DBName = "DB1";

	Subsystem1_Manager::mgr.Scan_rel();
	Subsystem1_Manager::mgr.Scan_attribute();
	Subsystem1_Manager::mgr.UserName = "User1";
	Subsystem1_Manager::mgr.DBName = "DB1";
	
	RBparse();

    PF_BufferMgr::pf_buffermgr.Write_all_buffer();
	Page_Mgr::page_mgr.Write_page();

	while (true);
	return 0;
}