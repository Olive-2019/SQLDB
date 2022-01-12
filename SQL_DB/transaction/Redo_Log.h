#pragma once
#include "Log.h"
class Redo_Log :
    public Log
{

private:
    void Redo();
    //vector<RID> rid;
    //vector<char*> new_content;
public:
    virtual void DO();

};

