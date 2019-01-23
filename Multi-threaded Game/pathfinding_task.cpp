// PathfindingTask: An astar algorithm task for calculating paths.
// James Kayes ©2016 
/////////////////////////////////////////////////////////////////////
#include "pathfinding_task.h"

PathfindingTask::PathfindingTask(Game* game, AStarFunction function, Grid* graph_copy, Tank* object, Vertex* start_node, Vertex* end_node): game_(game), task_(function), graph_copy_(graph_copy), start_node_(start_node), end_node_(end_node), object_(object)
{
	// We use these variables to find the corresponding vertices in the graphs copy:
	start_x = start_node_->coordinates_.x;
	start_y = start_node_->coordinates_.y;
	end_x = end_node_->coordinates_.x;
	end_y = end_node_->coordinates_.y;
}

PathfindingTask::~PathfindingTask()
{
}

void PathfindingTask::Run()
{
	// Call the function (value returned sets the path of the tasks object):
	if (object_ != nullptr)
	{
		auto objects_ready_for_deletion = game_->GetDeletionContainer();
		// If not ready for deletion:
		if (objects_ready_for_deletion->find(object_) == objects_ready_for_deletion->end())
		{
			object_->SetPath(graph_copy_, task_(game_, (*graph_copy_)[start_x][start_y], (*graph_copy_)[end_x][end_y])); // Set path.
		}
	}
}
