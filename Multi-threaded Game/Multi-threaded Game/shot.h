#pragma once
#include "game_object.h"
class Shot :
	public GameObject
{
protected:
	const sf::Vector2f velocity;
	const sf::Vector2f initial_position;
	sf::Vector2f position;
	set<GameObject*>* tanks;
	float traveled_squared;

public:
	Shot(sf::Texture &texture, Game* game_ptr, sf::Vector2f &position, sf::Vector2f &direction);
	virtual ~Shot();

	void Update(const sf::Time dt);


};

