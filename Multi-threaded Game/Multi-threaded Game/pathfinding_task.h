// PathfindingTask: An astar algorithm task for calculating paths.
// James Kayes ©2016 
//////////////////////////////////////////////////////////////////
#pragma once
#include <list>
#include <future>
#include "vertex.h"
#include "tank.h"
#include "task.h"

using std::list;

class Tank;
class Game;
typedef std::_Mem_fn<std::list<Vertex*, std::allocator<Vertex*>>(Game::*)(Vertex* start, Vertex* end)> AStarFunction;

class PathfindingTask :
	public Task
{
private:
	AStarFunction task_; // Pointer to actual astar member function.
	Game* game_;
	Tank* object_; // This could be extended to work with other pathfinding objects by creating another abstract class and extending to various other types.
	Grid graph_copy_; // Works with its own copy of the graph, for safety reasons.
	Vertex* start_node_;
	Vertex* end_node_;
	int start_x, start_y, end_x, end_y;

public:
	PathfindingTask(Game* game, AStarFunction function, Grid graph_copy, Tank* object, Vertex* start_node, Vertex* end_node);
	virtual ~PathfindingTask();

	void Run();
	
};

