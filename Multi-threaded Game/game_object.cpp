#include "game_object.h"

GameObject::GameObject(): current_game(nullptr)
{
}

GameObject::~GameObject()
{
}

void GameObject::SetSprite(sf::Sprite &sprite)
{
	object_sprite = sprite;
}

sf::Sprite* GameObject::GetSprite()
{
	return &object_sprite;
}

void GameObject::SetPosition(float x, float y)
{
}

void GameObject::SetPosition(sf::Vector2<float>& position)
{
	object_sprite.setPosition(position);
}

sf::Vector2<float> GameObject::GetPosition()
{
	return object_sprite.getPosition();
}

float GameObject::GetAngle()
{
	return object_sprite.getRotation();
}

void GameObject::ChangeAngle(const float angle)
{
	object_sprite.setRotation(GetAngle() + angle);
}
