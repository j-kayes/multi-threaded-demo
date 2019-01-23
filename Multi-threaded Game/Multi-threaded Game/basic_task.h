// BasicTask: A task with no parameters, returning void.
// James Kayes ©2016 
////////////////////////////////////////////////////////
#pragma once
#include "task.h"

class BasicTask :
	public Task
{
private:
	std::function<void()> task_;

public:
	BasicTask(std::function<void()>& function);
	virtual ~BasicTask();

	virtual void Run();

};

