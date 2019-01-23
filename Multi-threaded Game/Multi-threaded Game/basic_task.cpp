// BasicTask: A task with no parameters, returning void.
// James Kayes ©2016 
/////////////////////////////////////////////////////////
#include "basic_task.h"

BasicTask::BasicTask(std::function<void()> &function)
{
	task_ = function;
}

BasicTask::~BasicTask()
{
}

void BasicTask::Run()
{
	task_(); // Call the function object.
	delete this;
}

