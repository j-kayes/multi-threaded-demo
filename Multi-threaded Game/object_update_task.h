// ObjectUpdate Task:	
// James Kayes ©2016 
////////////////////////////
#pragma once
#include "task.h"
#include"game.h"

class GameObject;
class ObjectUpdateTask :
	public Task
{
private:
	GameObject* object_;
	sf::Time dt_;

public:
	ObjectUpdateTask(GameObject* object, sf::Time dt);
	virtual ~ObjectUpdateTask();

	void Run();
};

