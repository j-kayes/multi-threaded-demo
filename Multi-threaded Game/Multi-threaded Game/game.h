#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <set>
#include <future>
#include <cassert>
#include <stdlib.h>
#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <functional>
#include <boost/thread/barrier.hpp>
#include <atomic>
#include "thread_manager.h"
#include "channel.h"
#include "game_constants.h"
#include "pathfinding.h"
#include "game_object.h"
#include "update_cell_task.h"
#include "shot_update_task.h"


using std::vector;
using std::list;
using std::set;

class GameObject;
class Game
{
private:
	ThreadManager* thread_manager;
	boost::barrier* update_barrier; // I could implement my own barrier class, but using boost here is quicker.
	Grid graph; // 26x20 grid/graph.
	Vertex *start_node;
	Vertex *end_node;
	std::list<Vertex*> current_path;
	sf::Clock game_timer;
	sf::RenderWindow window;
	sf::RectangleShape squares[26][20];
	sf::Font font;
	sf::Text text_money;
	sf::Text text_health;
	sf::Texture texture_tank, texture_turret, texture_shot;
	std::string str_money;
	std::string str_health;
	Channel<sf::RectangleShape*> square_channel; // Channel used to transfer data to main thread.
	vector<sf::RectangleShape> path_line;
	set<GameObject*> tanks;
	set<GameObject*> shots;
	set<GameObject*> turrets;
	set<GameObject*> scheduled_deletion;

	float path_length;
	float algorithm_duration;
	float tank_spawn_timer;
	bool start_selected;
	bool end_selected; 
	bool path_found;
	int start_x, start_y;
	int end_x, end_y;

	std::atomic<int> money;
	std::atomic<int> health;
	unsigned int earnings;
public:
	Game();
	~Game();

	void Run();
	void Update(sf::Time dt);
	Grid InitialiseGrid();
	Grid CopyGrid();
	void Draw();
	void UpdateSquare(int x, int y);
	void DecrementHealth();
	void AddMoney(const unsigned int ammount);
	void ClearGrid();
	list<Vertex*> AStarAlgorithm(Vertex* start_node, Vertex* end_node); 
	float ManhattanDistance(Vertex* node); // Heuristic.
	void UpdatePaths();
	Vertex* GetStartNode();
	Vertex* GetEndNode();
	std::list<Vertex*>* GetPath(); // Returns pointer to current path.
	Grid* GetGraph();
	sf::RenderWindow* GetWindow();
	ThreadManager* GetThreadManager();
	set<GameObject*>* GetTanks();
	set<GameObject*>* GetShots();
	float GetTankRespawnTime();
	void RemoveGameObject(GameObject* object);
	void ScheduledDeletion();
	sf::Vector2<float> GetPositionFromCell(int x, int y);
	sf::Texture& shot_texture();

};

