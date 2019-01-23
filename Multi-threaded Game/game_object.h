#pragma once
#include "game.h"

class Game;
class GameObject
{
protected:
	Game* current_game; // Reference to the currently running game.
	sf::Sprite object_sprite;

public:
	GameObject();
	virtual ~GameObject();

	virtual void SetSprite(sf::Sprite &sprite);
	virtual sf::Sprite* GetSprite();
	virtual void SetPosition(float x, float y);
	virtual void SetPosition(sf::Vector2<float> &position);
	virtual sf::Vector2<float> GetPosition();
	virtual void ChangeAngle(const float angle);
	virtual float GetAngle();
	virtual void Update(const sf::Time dt) = 0;
};

