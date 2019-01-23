#include "shot.h"
#include "tank.h"

Shot::Shot(sf::Texture &texture, Game* game_ptr, sf::Vector2f &position, sf::Vector2f &direction) : velocity(kShotSpeed*direction), initial_position(position), traveled_squared(0)
{
	object_sprite.setTexture(texture);
	object_sprite.setOrigin(object_sprite.getLocalBounds().width / 2, object_sprite.getLocalBounds().height / 2);
	SetPosition(position);
	current_game = game_ptr;
}

Shot::~Shot()
{
}

void Shot::Update(const sf::Time dt)
{
	position = GetPosition();
	object_sprite.move(velocity*dt.asSeconds());
	traveled_squared = powf((initial_position.x - position.x), 2.0f) + powf((initial_position.y - position.y), 2.0f);
	if (traveled_squared >= kTurretRangeSquared)
	{
		current_game->RemoveGameObject(this);
		return;
	}

	tanks = current_game->GetTanks();
	// Don't actually need to protect this as the tanks are not removed until the start of the next update loop, and the tanks health is represented with an atomic type:
	for (auto tank : *tanks)
	{
		// Shot/Tank collision detection:
		if ((position.x > tank->GetPosition().x - kTankHitWidth) && (position.x < tank->GetPosition().x + kTankHitWidth))
		{
			if ((position.y > tank->GetPosition().y - kTankHitHeight) && (position.y < tank->GetPosition().y + kTankHitHeight))
			{
				static_cast<Tank*>(tank)->Damage(kShotDamage);
				current_game->RemoveGameObject(this);
				return;
			}
		}
	}
}
