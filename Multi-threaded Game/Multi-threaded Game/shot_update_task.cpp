#include "shot_update_task.h"
#include "shot.h"

ShotUpdateTask::ShotUpdateTask(Shot* shot, sf::Time dt): shot_(shot), dt_(dt)
{
}

ShotUpdateTask::~ShotUpdateTask()
{
}

void ShotUpdateTask::Run()
{
	shot_->Update(dt_);
}
