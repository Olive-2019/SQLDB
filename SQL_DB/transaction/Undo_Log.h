#pragma once
#include "Log.h"
#include <string>
using namespace std;
class Undo_Log :
    public Log
{
private:
    void Undo();
    string RSQL;
public:
    virtual void DO();
};

