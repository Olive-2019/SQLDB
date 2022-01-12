#pragma once
#include <string>
#include "Lock.h"
#include <vector>
using namespace std;
class Transaction
{
private:
	int ID;
	vector<string> SQLs;
	string current_SQL;
	vector<Lock*> Locks;
public:
	void start_transaction();
	void execute_SQL();
	void commit_transaction();
	void restore();

};

