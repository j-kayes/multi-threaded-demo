#include "tank.h"

Tank::Tank(sf::Texture &texture, Game* game_ptr, ThreadManager* manager): thread_manager(manager),
	graph_copy_(nullptr), health(kTankInitialHealth), speed(kTankSpeed), movement_direction(RIGHT), cell_x(-1), cell_y(9)
{
	object_sprite.setTexture(texture);
	object_sprite.setOrigin(object_sprite.getLocalBounds().width / 2, object_sprite.getLocalBounds().height / 2);
	current_game = game_ptr;
	current_node = game_ptr->GetStartNode();
	next_node = current_node;
	SetPosition(current_game->GetPositionFromCell(cell_x, cell_y));
	last_position = object_sprite.getPosition();
	current_map = current_game->GetGraph();
}

Tank::~Tank()
{
	DeleteCopy(); // Deletes tanks copy of graph to ensure no memmory leaks.
}

void Tank::Update(const sf::Time dt)
{
	current_position = object_sprite.getPosition(); // Update current_postion.
	if (current_position.x > current_game->GetWindow()->getSize().x) 
	{
		// Delete if off screen:
		current_game->DecrementHealth();
		current_game->RemoveGameObject(this);
		return;
	}
	if (health < 0)
	{
		// Destroy tank:
		current_game->AddMoney(kTankIncome);
		current_game->RemoveGameObject(this);
		return;
	}
	if (current_position.x - last_position.x >= 36.0f) // Tank cell changed.
	{
		cell_x++; // x-cell increased by one.	
		
		UpdateCell();
		SetPosition(current_game->GetPositionFromCell(cell_x, cell_y));

		last_position = object_sprite.getPosition(); // Set this as the last position.
		UpdateDirection();
	}
	if (current_position.x - last_position.x <= -36.0f) // Tank cell changed.
	{
		cell_x--; // x-cell decreased by on.
		current_node = (*current_map)[cell_x][cell_y]; // Update current node from the cell values.

		UpdateCell();
		SetPosition(current_game->GetPositionFromCell(cell_x, cell_y));
		last_position = object_sprite.getPosition(); // Set this as the last position.
		UpdateDirection();
	}
	if (current_position.y - last_position.y >= 36.0f) // Tank cell changed.
	{
		cell_y++; // y-cell increased by one.
		current_node = (*current_map)[cell_x][cell_y]; // Update current node from the cell values.

		UpdateCell();
		SetPosition(current_game->GetPositionFromCell(cell_x, cell_y));
		last_position = object_sprite.getPosition(); // Set this as the last position.
		UpdateDirection();
	}
	if (current_position.y - last_position.y <= -36.0f) // Tank cell changed.
	{
		cell_y--; // y-cell decreased by one.
		current_node = (*current_map)[cell_x][cell_y]; // Update current node from the cell values.

		UpdateCell();
		SetPosition(current_game->GetPositionFromCell(cell_x, cell_y));
		last_position = object_sprite.getPosition(); // Set this as the last position.
		UpdateDirection();
	}
	std::unique_lock<std::mutex> lock(path_mutex);
	if (movement_path.size() > 0)
	{
		lock.unlock();
		switch (movement_direction)
		{
		case UP:
			object_sprite.move(sf::Vector2<float>(0.0f, -speed*dt.asSeconds()));
			object_sprite.setRotation(0.0f);
			break;
		case RIGHT:
			object_sprite.move(sf::Vector2<float>(speed*dt.asSeconds(), 0.0f));
			object_sprite.setRotation(90.0f);
			break;
		case DOWN:
			object_sprite.move(sf::Vector2<float>(0.0f, speed*dt.asSeconds()));
			object_sprite.setRotation(180.0f);
			break;
		case LEFT:
			object_sprite.move(sf::Vector2<float>(-speed*dt.asSeconds(), 0.0f));
			object_sprite.setRotation(270.0f);
			break;
		case STOP:
			break; // Dont move.
		default:
			assert(false); // ERROR.
			break;
		}
	}
}

inline void Tank::UpdateCell()
{
	current_map = current_game->GetGraph(); // Update map.
	current_node = (*current_map)[cell_x][cell_y]; // Update current node using current cell values.
}

void Tank::UpdateDirection()
{
	if (*current_node == *current_game->GetEndNode() || *current_node == *current_game->GetStartNode())
	{
		movement_direction = RIGHT; // Move right so that the tank moves off screen.
		return;
	}
	std::unique_lock<std::mutex> lock(path_mutex); // We need to lock here as the path can be updated by threads running pathfinding tasks.
	if (movement_path.size() != 0)
	{
		for (auto it = movement_path.begin(); it != movement_path.end(); ++it)
		{
			Vertex* node = *it;
			if (*node == *current_node)
			{
				next_node = *(++it); // Set this to the next in the path.
			}
		}
	}
	else
	{
		next_node = current_node;
	}
	lock.unlock();
	// Calculate direction from current to next node:
	if (next_node == current_node) // No path.
	{
		movement_direction = STOP;
	}
	else if ((next_node->coordinates_.x - current_node->coordinates_.x) > 0)
	{
		movement_direction = RIGHT;
	}
	else if ((next_node->coordinates_.x - current_node->coordinates_.x) < 0)
	{
		movement_direction = LEFT;
	}
	else if ((next_node->coordinates_.y - current_node->coordinates_.y) < 0)
	{
		movement_direction = UP;
	}
	else if ((next_node->coordinates_.y - current_node->coordinates_.y) > 0)
	{
		movement_direction = DOWN;
	}

}

Vertex* Tank::GetCurrentNode()
{
	return current_node;
}

void Tank::SetPath(Grid* graph_copy, std::list<Vertex*> path)
{
	std::unique_lock<std::mutex> lock(path_mutex); // This unlocks when out of scope.
	if (graph_copy_ != nullptr)
	{
		DeleteCopy(); // Delete old copy.
		graph_copy_ = nullptr;
	}
	graph_copy_ = graph_copy; // Set to new copy.
	movement_path = path;
	/*if (path.size() != 0)
	{
		SetPosition(current_game->GetPositionFromCell(path.front()->coordinates_.x, path.front()->coordinates_.y));
	}*/
}

void Tank::Damage(const unsigned int ammount)
{
	health -= ammount;
}

// Deletes all of the elements of this tanks graph copy, to ensure no memory leaks:
void Tank::DeleteCopy()
{
	for (auto graph_line : *graph_copy_)
	{
		for (auto graph_vertex : graph_line)
		{
			delete graph_vertex;
		}
	}
	delete graph_copy_;
}
