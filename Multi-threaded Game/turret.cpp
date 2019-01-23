#include "turret.h"
#include "tank.h"

Turret::Turret(sf::Texture &texture, Game *game_ptr, int x, int y) : cell_x(x), cell_y(y), 
		to_target(sf::Vector2f(0.0f, 1.0f)), angle_to_target(0.0f), target(nullptr),
		shoot_timer(0.0f)
{
	object_sprite.setTexture(texture);
	object_sprite.setOrigin(object_sprite.getLocalBounds().width / 2, object_sprite.getLocalBounds().height / 2);
	current_game = game_ptr;
	SetPosition(current_game->GetPositionFromCell(cell_x, cell_y));
	object_sprite.setRotation(0.0f);
}

Turret::~Turret()
{
}

void Turret::Update(const sf::Time dt)
{
	tanks = current_game->GetTanks(); // Get tank objects.
	shots = current_game->GetShots(); // Get shot objects.

	// Can only shoot if this value is 0:
	if (shoot_timer > 0.0f)
	{
		shoot_timer -= dt.asSeconds(); // Decrement timer.
	}

	// If there are objects in range:
	if (!objects_in_range.empty())
	{
		// Loop over them:
		for (auto it = objects_in_range.begin(); it != objects_in_range.end();)
		{
			if (auto tank = *it)
			{
				auto scheduled_deletion = current_game->GetDeletionContainer();
				if (scheduled_deletion->find(*it) != scheduled_deletion->end())
				{
					distance_squared = powf((tank->GetPosition().x - this->GetPosition().x), 2.0f) + powf((tank->GetPosition().y - this->GetPosition().y), 2.0f);
					if (distance_squared > kTurretRangeSquared)
					{
						it = objects_in_range.erase(it); // Remove if out of range.
					}
					else
					{
						++it;
					}
				}
				else
				{
					it = objects_in_range.erase(it); // Remove if ready for deletion.
				}
			}
			else
			{
				// Tank destroyed:
				it = objects_in_range.erase(it);
			}
		}
	}
	// Loop over tanks:
	for (auto tank : *tanks)
	{
		distance_squared = powf((tank->GetPosition().x - this->GetPosition().x), 2.0f) + powf((tank->GetPosition().y - this->GetPosition().y), 2.0f);
		if (distance_squared < kTurretRangeSquared)
		{
			objects_in_range.push_back(tank); // Add if in range.
		}

	}
	// If objects are in range, and no target:
	if (!objects_in_range.empty() && target == nullptr)
	{
		target = objects_in_range[0]; // Set target to first in vector.
	}
	else
	{
		target = nullptr; // No target, if there are objects in range target will be set at the next update.
	}
	if (target != nullptr) // If target set:
	{
		// If target has been destroyed:
		if (tanks->find(target) == tanks->end())
		{
			// Find and remove from objects_in_range:
			for (auto it = objects_in_range.begin(); it != objects_in_range.end(); ++it)
			{
				if (target == *it)
				{
					objects_in_range.erase(it);
					break;
				}
			}
			target = nullptr; // Set to no target.
			return; 
		}
		// Calculate vector to target:
		to_target = target->GetPosition() - this->GetPosition();

		angle_to_target = RADS_TO_DEGS(CalculateTargetAngle()); 

		// Rotate to target:
		if ((angle_to_target >= 270.0f) && (GetAngle() <= 90.0f))
		{
			// If target in top-left quadrant, AND turret is facing the top-right quadrant:
			RotateAntiClockwise(dt);
		}
		else if ((angle_to_target <= 90.0f) && (GetAngle() >= 270.0f))
		{
			// If target in top-right quadrant, AND turret is facing the top-left quadrant:
			RotateClockwise(dt);
		}
		else if (angle_to_target < (GetAngle() - kTurretAimAngle))
		{
			RotateAntiClockwise(dt);
		}
		else if (angle_to_target > (GetAngle() + kTurretAimAngle))
		{
			RotateClockwise(dt);
		}
		else // Aiming in target direction:
		{
			// SHOOT STATE:
			if (shoot_timer <= 0.0f) // If shoot timer at 0
			{
				shots->insert(static_cast<GameObject*>(Shoot()));
				shoot_timer = 1 / kTurretShootRate; // Reset timer.
			}
		}
	}
}

void Turret::RotateClockwise(const sf::Time dt)
{
	ChangeAngle(kTurretRotationSpeed*dt.asSeconds());
}

void Turret::RotateAntiClockwise(const sf::Time dt)
{
	ChangeAngle(-kTurretRotationSpeed*dt.asSeconds());
}

int Turret::GetX()
{
	return cell_x;
}

int Turret::GetY()
{
	return cell_y;
}

Shot* Turret::Shoot()
{
	return new Shot(current_game->shot_texture(), current_game, GetPosition(), sf::Vector2f(sin(DEGS_TO_RADS(GetAngle())), -cos(DEGS_TO_RADS(GetAngle()))));
}

float Turret::CalculateTargetAngle()
{
	if (to_target.x < 0) // Target to the left.
	{
		if (to_target.y == 0) // Left.
		{
			return (3 / 2)*kPi; // 270 degrees.
		}
		else if (to_target.y == 1)
		{
			return 0; // 0 degrees.
		}
		else if (to_target.y < 0) // Top Left.
		{
			return 2 * kPi - atan(to_target.x / to_target.y);
		}
		else if (to_target.y > 0) // Bottom Left.
		{
			return (3 / 2)*kPi - atan(to_target.x / to_target.y);
		}
	}
	else // To the right or 0.
	{
		if (to_target.y == 0) // Right.
		{
			return kPi / 2; // 90 degrees.
		}
		else if (to_target.y == -1)
		{
			return kPi; // 180 degrees.
		}
		else if (to_target.y < 0) // Top Right.
		{
			return -atan(to_target.x / to_target.y);
		}
		else if (to_target.y > 0) // Bottom right.
		{
			return kPi - atan(to_target.x / to_target.y);
		}
	}
	return 0.0f;
}
