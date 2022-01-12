#pragma once
#include "Transaction.h"
#include <vector>
using namespace std;
class Transaction_Manager
{
private:
	vector<Transaction*> current_transaction;
public:
	Transaction_Manager();
	Transaction* get_transaction();

};

