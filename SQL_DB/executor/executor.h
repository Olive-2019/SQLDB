#pragma once
#include "../optimizer/optimizer.h"
#include "../RECORD.h"
class Executor {
public:
	Executor(Optimizer* optimizer);
	void execute();
private:

	vector<Execution_Plan> Plans;
	void Display();
	void Insert();
	void Delete();
	void Update();

	void Link();

	bool Check_Record(RECORD &Lrecord,RECORD &Rrecord, Condition cond);

};