// ObjectUpdate Task:	
// James Kayes ©2016 
///////////////////////
#include "object_update_task.h"
#include "game_object.h"

ObjectUpdateTask::ObjectUpdateTask(GameObject* object, sf::Time dt) : object_(object), dt_(dt)
{
}

ObjectUpdateTask::~ObjectUpdateTask()
{
}

void ObjectUpdateTask::Run()
{
	object_->Update(dt_);
}