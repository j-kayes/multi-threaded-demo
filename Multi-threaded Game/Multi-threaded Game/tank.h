#pragma once
#include <cassert>
#include <mutex>
#include "pathfinding_task.h"
#include "game_object.h"

enum Direction{
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Tank :
	public GameObject
{
protected:
	ThreadManager* thread_manager;
	const float speed;
	sf::Vector2<float> current_position;
	sf::Vector2<float> last_position;
	Direction movement_direction;
	int cell_x, cell_y; 
	Vertex* current_node; // Node that the tank is currently on.
	Vertex* next_node; // Next node that the tank has to head to.
	std::mutex path_mutex;  // Mutex for safely updating and using the path data between threads.
	std::list<Vertex*> movement_path; // Current list of verticies to end.
	Grid* current_map;
	int health;

	inline void UpdateCell();

public:
	Tank(sf::Texture& texture, Game* game_ptr, ThreadManager* manager);
	virtual ~Tank();

	void Update(const sf::Time dt);
	void UpdateDirection();
	Vertex* GetCurrentNode();
	void SetPath(std::list<Vertex*> path);
	void Damage(const unsigned int ammount);

};

