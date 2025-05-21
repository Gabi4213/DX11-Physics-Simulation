#include "DebugHelper.h"

DebugHelper::DebugHelper(int startValue)
{
	_startValue = startValue;
}
DebugHelper::~DebugHelper()
{
}

void DebugHelper::PrintDebug() 
{
	static int x = _startValue;
	char sz[1024] = { 0 };
	sprintf_s(sz, "the number is %d \n", x);
	OutputDebugStringA(sz);
	x++;
}

