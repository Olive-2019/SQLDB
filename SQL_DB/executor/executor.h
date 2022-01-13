#pragma once
#include "../optimizer/optimizer.h"
#include "../RECORD.h"
#include "../optimizer/Logical_Tree_Builder.h"
class Executor {
public:
	Executor(Logical_TreeNode* Root);
	void execute();
private:
	Logical_TreeNode* Root;
	void Display();
	void Insert();
	void Delete();
	void Update();
	void Link();
	bool Check_Record(RECORD &Lrecord,RECORD &Rrecord, Condition cond);
};