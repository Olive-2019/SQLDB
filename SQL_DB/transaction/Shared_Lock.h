#pragma once
#include "Lock.h"
class Shared_Lock :
    public Lock
{

public:
    virtual bool lock();
    virtual void unlock();

};

