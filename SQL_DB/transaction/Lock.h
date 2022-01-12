#pragma once
#include <map>
using namespace std;
enum Lock_Type {
	NONE,SHARED,EXCLUSIVE
};

struct Lock_Info {
	//���ݱ�
	//RID
	//�û�
	//DB
};


class Lock
{

protected:
	Lock_Info Info;
	static Lock_Type lookup_lock(Lock_Info& Info);
	static map<Lock_Info, Lock_Type> Is_Locked_Map;
	static bool Detect_Deadlock();
	static void Release_Deadlock();

public:
	Lock();
	virtual bool lock();
	virtual void unlock();

};

