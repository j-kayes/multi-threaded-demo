// Update Cell Task: Task for updating cell data, and getting user input from its position.
// James Kayes ©2016 
///////////////////////////////////////////////////////////////////////////////////////////
#include "update_cell_task.h"

UpdateCellTask::UpdateCellTask(Game* game, UpdateCellFunction function, int x, int y): game_(game), task_(function), x_(x), y_(y)
{
}

UpdateCellTask::~UpdateCellTask()
{
}

void UpdateCellTask::Run()
{
	task_(game_, x_, y_);
}
