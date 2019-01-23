// Multi-threaded demo
// James Kayes ©2016
/////////////////////////////////////////////////////////////////////////////////////////////
#include "game_object.h"
#include "tank.h"
#include "pathfinding.h"
#include "turret.h"
#include "shot.h"
#include "game.h"
#include "object_update_task.h"

// This can be adjusted acording to the hardware:
#define WORKER_THREADS 3  // Number of extra worker threads (Total threads = workers + main thread).

Game::Game() : window(sf::VideoMode(936, 720), "Multi-threaded Tower Defence Demo", sf::Style::Close), start_node(nullptr), end_node(nullptr), start_selected(false), end_selected(false),
	path_found(false), path_length(0), start_x(0), start_y(9), end_x(25), end_y(9), tank_spawn_timer(kTankInitialSpawnTime), health(kInitialHealth), money(kInitialMoney),
	record_times(false), ready(true)
{
	// This is used to wait for all worker threads to finish their tasks before the next frame:
	update_barrier = new Barrier(WORKER_THREADS + 1); // Worker threads + the main thread.

	// Thread manager requires at least one worker thread to run:
	thread_manager = new ThreadManager(update_barrier, ready_cv, ready, WORKER_THREADS); // Initialises thread manager class with the passed in number of threads.
	// You can then add tasks to this and it will divide work among the other threads.

	graph = InitialiseGrid();
	start_node = graph[start_x][start_y];
	end_node = graph[end_x][end_y];
	// Declare and load a font:
  	if (!font.loadFromFile("arial.ttf"))
	{
		exit(-2);
	}
	// Load textures:
	if (!texture_tank.loadFromFile("Sprites/tank.png"))
	{ 
		exit(-3);
	}
	texture_tank.setSmooth(true);
	if (!texture_turret.loadFromFile("Sprites/turret.png"))
	{
		exit(-3);
	}
	texture_turret.setSmooth(true);
	if (!texture_shot.loadFromFile("Sprites/shot.png"))
	{
		exit(-3);
	}
	texture_shot.setSmooth(true);

	text_money.setFont(font);
	text_money.setFillColor(sf::Color::Green);

	text_health.setFont(font);
	text_health.setFillColor(sf::Color::Red);

}

Grid Game::InitialiseGrid()
{
	Grid graph(26, std::vector<Vertex*>(20));
	// Fills grid:
	for (UInt32 h = 0; h < 20; h++)
	{
		for (UInt32 w = 0; w < 26; w++)
		{
			graph[w][h] = new Vertex(w, h);
		}
	}
	// Connects horizontal/verticals:
	for (UInt32 h = 0; h < 20; h++)
	{
		for (UInt32 w = 0; w < 26; w++)
		{
			if (w < 25)
			{
				Connect(*graph[w][h], *graph[w + 1][h], 1); // Connects horizontals.
			}
			if (h < 19)
			{
				Connect(*graph[w][h], *graph[w][h + 1], 1); // Connects verticals.
			}
		}
	}
	return graph;
}

// Returns a pointer to a copy of the grid, so that seperate threads can work on the same data without blocking:
Grid* Game::CopyGrid()
{
	Grid* graph_copy = new Grid(26, std::vector<Vertex*>(20));
	// Fill with new verticies:
	for (UInt32 h = 0; h < 20; h++)
	{
		for (UInt32 w = 0; w < 26; w++)
		{
			(*graph_copy)[w][h] = new Vertex(w, h); // Initialise new verticies for the copy.
		}
	}
	// We then need to loop over each node to update its connections:
	for (UInt32 h = 0; h < 20; h++)
	{
		for (UInt32 w = 0; w < 26; w++)
		{
			
			Vertex* original_node = graph[w][h]; // Get the corresponding node from the main graph.
			// Get connection data from main graph:
			for (auto connection : original_node->connections)
			{
				auto connection_coordinates = connection.node->coordinates_; // We use this to find the corresponding vertex in the graphs copy.
				// Push data into nodes connections vector for the copy:
				(*graph_copy)[w][h]->connections.push_back(Connection((*graph_copy)[connection_coordinates.x][connection_coordinates.y], connection.distance));
			}
			if (original_node->blocked)
			{
				(*graph_copy)[w][h]->blocked = true;
			}
		}
	}
	return graph_copy;
}

void Game::Run()
{
	// MAIN LOOP:
	while (window.isOpen())
	{
		Update(game_timer.restart()); // .restart() returns the elapsed time since last start.
	}
}

void Game::Update(sf::Time dt)
{
	ScheduledDeletion(); // This is better than deleting objects mid-loop (allows game object update functions to run concurrently).
	if (tank_spawn_timer > 0)
	{
		tank_spawn_timer -= dt.asSeconds(); // Decrement timer.
	}
	else if (tank_spawn_timer <= 0)
	{
		// Spawn new tank:
		Tank* new_tank = new Tank(texture_tank, this, thread_manager);
		tanks.insert(new_tank); 
		// Add new pathfinding task:
		thread_manager->AddTask(new PathfindingTask(this, std::mem_fn(&Game::AStarAlgorithm), CopyGrid(), new_tank, new_tank->GetCurrentNode(), end_node));
		tank_spawn_timer = kTankRespawnTime; // Reset spawn timer.
	}
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			// Cleanup data:
			delete thread_manager;
			delete start_node;
			delete end_node;
			delete update_barrier;
			window.close();
			exit(0);
		}
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Right)
			{
				// Spawn new tank:
				Tank* new_tank = new Tank(texture_tank, this, thread_manager);
				tanks.insert(new_tank);
				// Add new pathfinding task:
				thread_manager->AddTask(new PathfindingTask(this, std::mem_fn(&Game::AStarAlgorithm), CopyGrid(), new_tank, new_tank->GetCurrentNode(), end_node));
			}
			if (event.key.code == sf::Keyboard::R)
			{
				record_times = true; // Turns off graphics and writes time data to file.
			}
		}
		if (event.type == sf::Event::KeyReleased)
		{
			if (event.key.code == sf::Keyboard::R)
			{
				record_times = false;
			}
		}
	}
	// Update objects:
	for (auto object : tanks)
	{
		thread_manager->AddTask(new ObjectUpdateTask(object, dt));
	}
	for (auto object : turrets)
	{
		thread_manager->AddTask(new ObjectUpdateTask(object, dt));
	}
	for (auto object : shots)
	{
		thread_manager->AddTask(new ObjectUpdateTask(object, dt));
	}
	// For each cell:
	for (UInt32 y = 0; y < 20; y++)
	{
		for (UInt32 x = 0; x < 26; x++)
		{
			// Add a new update square task to the task manager:
			thread_manager->AddTask(new UpdateCellTask(this, std::mem_fn(&Game::UpdateSquare), x, y));
		}
	}
	// The ready bool allows the condition variable to be signaled in advance:
	while (!ready)
	{
		ready_cv.wait_for(std::unique_lock<std::mutex>(ready_cv_mutex), std::chrono::milliseconds(5));
	}
	ready = false; 
	thread_manager->RunTasks(); // This will start working on any tasks in parallel.
	Draw(); // Main thread draws.
	thread_manager->Signal(); // This just makes sure all tasks can end smoothly.
	update_barrier->Wait();
	if (record_times)
	{
		thread_manager->write_time(); // Record time to complete tasks in timings.txt.
	}
	window.display();
}


void Game::Draw()
{
	if (!record_times)
	{
		window.clear(sf::Color(0xF9, 0xF9, 0xF9));
	}
	// Once for each cell:
	for (int i = 0; i < 520; i++)
	{
		if ((thread_manager->GetNumberOfTasks() > 0) && (thread_manager->GetSemaphoreCount() == 0))
		{
			thread_manager->Signal(); // Helps prevent thread manager from jamming.
		}
     	auto square = square_channel.Read(); // This will wait for data to become available as the tasks complete.
		if (!record_times)
		{
			window.draw(*square); // SFML does allow drawing on multiple threads, but this is still a good demonstration of the producer consumer relationship discussed in the lectures.
		}
	}

	// Draw game objects:
	if (!record_times)
	{
		for (auto tank : tanks)
		{
			window.draw(*tank->GetSprite());
		}
		for (auto turret : turrets)
		{
			window.draw(*turret->GetSprite());
		}
		for (auto shot : shots)
		{
			window.draw(*shot->GetSprite());
		}
	}

	// TEXT:
	str_money = std::to_string(money);
	str_health = std::to_string(health);

	text_money.setString("Money: $" + str_money);
	text_money.setPosition(sf::Vector2f(20.0f, 10.0f));

	text_health.setString("Health: " + str_health);
	text_health.setPosition(sf::Vector2f(window.getSize().x - 160.0f, 10.0f));
	
	if (!record_times)
	{
		window.draw(text_money);
		window.draw(text_health);
	}
}

// Updates an individual squares data (task function):
void Game::UpdateSquare(int x, int y)
{
	squares[x][y].setPosition(sf::Vector2f(x*36.0f, y*36.0f));
	squares[x][y].setSize(sf::Vector2f(35.0f, 35.0f));
	squares[x][y].setOutlineThickness(1.0f);
	squares[x][y].setOutlineColor(sf::Color(0x44, 0x44, 0x44, 0x66));

	if (y == 0 || y == 19 || x == 0 || x == 25) // Outline.
	{
		graph[x][y]->blocked = true;
		squares[x][y].setFillColor(colour_blocked);
	}
	if (x == start_x && y == start_y)
	{
		graph[x][y]->blocked = false;
		squares[x][y].setFillColor(sf::Color::Transparent);
		start_node = graph[x][y];
	}
	else if (x == end_x && y == end_y)
	{
		graph[x][y]->blocked = false;
		squares[x][y].setFillColor(sf::Color::Transparent);
		end_node = graph[x][y];
	}
	else if (squares[x][y].getFillColor() != colour_blocked)
	{
		if ((squares[x][y].getFillColor() != colour_open_set) && (squares[x][y].getFillColor() != colour_closed_set))
		{
			squares[x][y].setFillColor(sf::Color::Transparent);
		}
	}
	// Get user input on the square:
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		if ((sf::Mouse::getPosition(window).x <= squares[x][y].getPosition().x + squares[x][y].getLocalBounds().width) && (sf::Mouse::getPosition(window).x >= squares[x][y].getPosition().x))
		{
			if ((sf::Mouse::getPosition(window).y <= squares[x][y].getPosition().y + squares[x][y].getLocalBounds().height) && (sf::Mouse::getPosition(window).y >= squares[x][y].getPosition().y))
			{
				// Turret placement:
				if (!graph[x][y]->blocked)
				{
					if (money >= kTurretBuyCost)
					{
						money -= kTurretBuyCost;
						turrets.insert(new Turret(texture_turret, this, x, y)); // Add turret at this position.
						graph[x][y]->blocked = true;
						// For each node that this connects to:
						std::unique_lock<std::mutex> connections_lock(connections_mutex);
						for (auto connection : graph[x][y]->connections)
						{
							// Update distances from the node so that the pathfinding algorithm prefers out of range paths:
							UpdateDistances(*connection.node, kTurretInRangeEdgeFactor);
						}
						connections_lock.unlock();
						UpdatePaths();
					}
				}
			}
		}
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		if ((sf::Mouse::getPosition(window).x <= squares[x][y].getPosition().x + squares[x][y].getLocalBounds().width) && (sf::Mouse::getPosition(window).x >= squares[x][y].getPosition().x))
		{
			if ((sf::Mouse::getPosition(window).y <= squares[x][y].getPosition().y + squares[x][y].getLocalBounds().height) && (sf::Mouse::getPosition(window).y >= squares[x][y].getPosition().y))
			{
				// Turret removal:
				if (graph[x][y]->blocked && !(y == 0 || y == 19 || x == 0 || x == 25)) // Blocked and not an edge.
				{
					for (auto object : turrets)
					{
						auto turret = static_cast<Turret*>(object);
						if ((turret->GetX() == x) && (turret->GetY() == y))
						{
							std::unique_lock<std::mutex> deletion_lock(deletion_mutex);
							scheduled_deletion.insert(turret); // This will then remove this object at the start of the next update.
							money += kTurretSellCost;
						}
					}
					graph[x][y]->blocked = false;
					std::unique_lock<std::mutex> connections_lock(connections_mutex);
					// For each node that this connects to:
					for (auto connection : graph[x][y]->connections)
					{
						// Reset distances from these nodes:
						UpdateDistances(*connection.node, 1.0f);
					}
					connections_lock.unlock();
					UpdatePaths();
				}
			}
		}
	}

	// Push data through channel:
	square_channel.Write(&squares[x][y]); // This is then consumed by the main thread.
}

void Game::DecrementHealth()
{
	health--;
}

void Game::AddMoney(const unsigned int ammount)
{
	money += ammount;
}

Vertex* Game::GetStartNode()
{
	return start_node;
}

Vertex* Game::GetEndNode()
{
	return end_node;
}

void Game::ClearGrid()
{
	for (int y = 0; y < 20; y++)
	{
		for (int x = 0; x < 26; x++)
		{
			if (squares[x][y].getFillColor() != colour_blocked)
			{
				squares[x][y].setFillColor(sf::Color::Transparent);
			}
		}
	}
}

list<Vertex*> Game::AStarAlgorithm(Vertex* start, Vertex* end)
{
	// compare_distances is a functor that orders the set by distance/f-cost, rather than address.
	std::set<Vertex*, compare_distances> open_set;
	std::set<Vertex*> closed_set;

	float current_distance = 0; // Distance to start node is 0.
	Vertex* current_node;
	Vertex* next_node; 
	start->g_cost = current_distance; 
	start->h_cost = ManhattanDistance(start);

	start->f_cost = start->g_cost + start->h_cost;
	start->parent = nullptr; // Start node has no parent
	bool no_path = false;

	open_set.insert(start);
	while (no_path == false)
	{
		current_node = *open_set.begin(); // This gives the node with the lowest f-cost as the set is sorted by distance/f-cost.
		current_distance = current_node->g_cost;
		if (current_node == end)
		{
			break; // We have found a path.
		}
		open_set.erase(open_set.begin());
		closed_set.insert(current_node); // Mark current node as visited/add it to the closed set.

		for (auto connection_ : current_node->connections) // Loop through all the vertex connections (neigbours).
		{
			next_node = connection_.node; // Get the node that this connection leads to
			// If this node has already been added to the closed set, OR its blocked:
			if ((closed_set.find(next_node) != closed_set.end()) || (next_node->blocked == true))
			{
				continue; // Move on to the next node.
			}
			else
			{
				float total_distance = current_distance + connection_.distance; // Calculate total distance to this node through the current_node.
				if (open_set.find(next_node) == open_set.end()) // If the node is NOT already in the open set.
				{
					// Update g/h/f costs then add it to the open set:
					next_node->g_cost = total_distance;
					next_node->h_cost = ManhattanDistance(next_node); // Estimated distance to end node from this node.
					next_node->f_cost = next_node->g_cost + next_node->h_cost;
					next_node->parent = current_node; // Set this node as its parent
					open_set.insert(next_node); // Add this node to the open set.
				}
				else if (total_distance < next_node->f_cost) // If this node IS in the open set and this path gives a shorter distance:
				{
					next_node->g_cost = total_distance; // Relax the distance.
					next_node->f_cost = next_node->g_cost + next_node->h_cost; // Recalculate f-cost.
					next_node->parent = current_node; // Set this node as its parent
				}
			}
		}
		if (open_set.empty())
		{
			no_path = true;
		}
	}  
	// Trace path.
	std::list<Vertex*> path;
	std::list<Vertex*> path_return;
	Vertex* path_node = end; // Current node being added to the path.
	if (no_path == false)
	{
		while (path_node != start)
		{
			if (path_node->parent == nullptr)
			{
				break;
			}
			path.push_front(path_node); // Add to path.
			path_node = path_node->parent; // Next node to add.
		}
		path.push_front(start);
		path_found = true;
		for (auto node : path)
		{
			path_return.push_back(new Vertex(node->coordinates_.x, node->coordinates_.y));
		}
	}
	else
	{
		path_found = false;
	}
	return path_return;
}

float Game::ManhattanDistance(Vertex *node)
{
	float dx = abs(static_cast<float>(node->coordinates_.x) - static_cast<float>(end_node->coordinates_.x));
	float dy = abs(static_cast<float>(node->coordinates_.y) - static_cast<float>(end_node->coordinates_.y));
	return dx + dy;
}

void Game::UpdatePaths()
{
	// For each tank:
	for (GameObject* object : tanks)
	{
		Tank* tank_object = static_cast<Tank*>(object);
		// This will add a new pathfinding task for the threadmanager, this will then find another thread for this to run on:
		thread_manager->AddTask(new PathfindingTask(this, std::mem_fn(&Game::AStarAlgorithm), CopyGrid(), tank_object, tank_object->GetCurrentNode(), end_node));
	}
}

std::list<Vertex*>* Game::GetPath()
{
	return &current_path;
}

Grid* Game::GetGraph()
{
	return &graph;
}

sf::RenderWindow* Game::GetWindow()
{
	return &window;
}

ThreadManager* Game::GetThreadManager()
{
	return thread_manager;
}

set<GameObject*>* Game::GetTanks()
{
	return &tanks;
}

set<GameObject*>* Game::GetShots()
{
	return &shots;
}

// Could be used to respawn the tanks at different rates depending on various factors in the game:
float Game::GetTankRespawnTime()
{
	float seconds = 7.0f - 0.12f*(earnings/100);
	if (seconds <= 0.5f)
	{
		return 0.5f;
	}
	return seconds;
}


void Game::RemoveGameObject(GameObject* object)
{
	std::unique_lock<std::mutex> deletion_lock(deletion_mutex);
	scheduled_deletion.insert(object);
}

void Game::ScheduledDeletion()
{
	unsigned int garbage = 0;
	for (auto object : scheduled_deletion)
	{
		try {
			if (Tank* tank = dynamic_cast<Tank*>(object))
			{
				tanks.erase(tank);
				delete tank;
			}
			else if (Shot* shot = dynamic_cast<Shot*>(object))
			{
				shots.erase(shot);
				delete shot;
			}
			else if (Turret* turret = dynamic_cast<Turret*>(object))
			{
				turrets.erase(turret);
				delete turret;
			}
		}
		catch (std::__non_rtti_object)
		{
			object = nullptr;
			garbage++;
			if (garbage >= scheduled_deletion.size())
			{
				break;
			}
		}
		object = nullptr;
	}
	scheduled_deletion.clear();
}

set<GameObject*>* Game::GetDeletionContainer()
{
	std::unique_lock<std::mutex> deletion_lock(deletion_mutex);
	return &scheduled_deletion;
}

// Returns the position vector of the center of the cell with the given x and y coordinates as passed in:
sf::Vector2<float> Game::GetPositionFromCell(int x, int y)
{
	return sf::Vector2<float>(x*36.0f + 18.0f, y*36.0f + 16.0f);
}

sf::Texture& Game::shot_texture()
{
	return texture_shot;
}

Game::~Game()
{
}
