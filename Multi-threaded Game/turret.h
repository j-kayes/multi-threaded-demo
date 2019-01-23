#pragma once
#include "game_object.h"
#include "game_constants.h"
#include "shot.h"
#include <set>
#include <vector>
#include <math.h>

#define DEGS_TO_RADS(degrees) ((degrees/180)*kPi)
#define RADS_TO_DEGS(radians) ((radians/kPi)*180)

using std::set;
using std::vector;

class Turret :
	public GameObject
{
private:
	int cell_x, cell_y;
	set<GameObject*>* tanks;
	set<GameObject*>* shots;
	vector<GameObject*> objects_in_range; // Vector of tanks in range (in order added, size shouldn't exceed 8 or so).
	GameObject* target;
	sf::Vector2<float> to_target;
	float angle_to_target;
	float distance_squared;
	float shoot_timer;

public:
	Turret(sf::Texture& texture, Game* game_ptr, int cell_x, int cell_y);
	virtual ~Turret();

	float CalculateTargetAngle();
	void Update(const sf::Time dt);
	void RotateClockwise(const sf::Time dt);
	void RotateAntiClockwise(const sf::Time dt);
	int GetX();
	int GetY();
	Shot* Shoot();
};

