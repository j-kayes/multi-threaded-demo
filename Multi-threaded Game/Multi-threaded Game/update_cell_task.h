#pragma once
#include "task.h"

class Game;
typedef std::_Mem_fn<void(Game::*)(int x, int y)> UpdateCellFunction;

class UpdateCellTask :
	public Task
{
private:
	UpdateCellFunction task_;
	Game* game_;
	int x_, y_;

public:
	UpdateCellTask(Game* game, UpdateCellFunction function, int x, int y);
	virtual ~UpdateCellTask();

	void Run();
};

