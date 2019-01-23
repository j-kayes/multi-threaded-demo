// Task Abstract Class: Base class for other task types.
// James Kayes ©2016 
/////////////////////////////////////////////////////////
#pragma once
#include <functional>

class Task
{
public:
	Task();
	virtual ~Task();

	virtual void Run() = 0;

};

