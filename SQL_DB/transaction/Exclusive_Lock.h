#pragma once
#include "Lock.h"
class Exclusive_Lock :
    public Lock
{
public:
    virtual bool lock();
    virtual void unlock();
};

