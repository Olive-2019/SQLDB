#pragma once
class Log_Manager
{
public:
	Log_Manager();

	void New_Log();
	void Redo();
	void Undo();
	void Add_Check_Point();
	void Restore();
};

