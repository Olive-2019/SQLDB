#include "global.h"
string Global_Paras:: Current_DB;
string Global_Paras:: Current_User;
int Global_Paras:: Block_Size;

int main() {
	Global_Paras::Current_DB = "test";
	cout << Global_Paras::Current_DB << endl;
}