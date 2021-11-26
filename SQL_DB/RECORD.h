#pragma once
#include "global.h"
class RECORD
{
private:

	char* record;

	vector<Attr_Info> Attrs;

public:
	char* get_Attr_Value(string attr_name);

};

