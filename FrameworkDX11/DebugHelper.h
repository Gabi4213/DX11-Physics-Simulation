#include <string>
#include <Windows.h>
#pragma once
class DebugHelper
{
private:
	int _startValue;
public:
	DebugHelper(int startValue);
	~DebugHelper();
	void PrintDebug();
};

