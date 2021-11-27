#include "Subsystem1.h"

Subsystem1_Manager Subsystem1_Manager::BASE;


bool Subsystem1_Manager::lookup_Rel(string RelName, Rel_Info& rel)
{
	return false;
}

bool Subsystem1_Manager::lookup_Attr(string RelName, string AttrName, Attr_Info& attr)
{
	return false;
}

vector<Attr_Info> Subsystem1_Manager::lookup_Attrs(string RelName)
{
	return vector<Attr_Info>();
}

bool* Subsystem1_Manager::lookup_Authority(string RelName, string UserName)
{
	return nullptr;
}

bool Subsystem1_Manager::lookup_Index(string RelName, string AttrName, Index_Info& Index)
{
	return false;
}


vector<Index_Info> Subsystem1_Manager::lookup_Indexes(string RelName)
{
	return vector<Index_Info>();
}

char* Subsystem1_Manager::Find_Record_by_RID(RID rid)
{
	return nullptr;
}

void Subsystem1_Manager::Insert_Reocrd(string RelName, char* record)
{
}

void Subsystem1_Manager::Delete_Record(string RelName, vector<RID> rids)
{
}

void Subsystem1_Manager::Update_Record(string RelName, RID rid, Attr_Info attr, char* new_value)
{
}

//Scan_Reader::Scan_Reader(string RelName)
//{
//
//}

char* Scan_Reader::get_Next_Record()
{
	return nullptr;
}

char* Scan_Reader::get_Next_Record_with_RID(RID& rid)
{
	return nullptr;
}


Index_Reader::Index_Reader(string RelName, Index_Info Index, bool MIN_E, char* MIN, bool MAX_E, char* MAX)
{
}

char* Index_Reader::get_Next_Record()
{
	return nullptr;
}

char* Index_Reader::get_Next_Record_with_RID(RID& rid)
{
	return nullptr;
}
