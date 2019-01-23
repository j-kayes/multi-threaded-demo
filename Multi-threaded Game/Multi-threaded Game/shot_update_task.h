#pragma once
#include "task.h"
#include "game.h"

class Shot;

class ShotUpdateTask :
	public Task
{
private:
	Shot* shot_;
	sf::Time dt_;

public:
	ShotUpdateTask(Shot* shot, sf::Time dt);
	virtual ~ShotUpdateTask();

	void Run();
};

