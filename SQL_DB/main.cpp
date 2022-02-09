#include "Subsystem1.h"
#include "global.h"
#include "parser/parser.h"

int main() {
	Subsystem1_Manager::mgr.Scan_rel();
	Subsystem1_Manager::mgr.Scan_attribute();
	Subsystem1_Manager::mgr.UserName = "User1";
	Subsystem1_Manager::mgr.DBName = "DB1";
	RBparse();



	while (true);
	return 0;
}