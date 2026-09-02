#include "Projectile.h"

void Projectile::playExplosion()
{
	if (isDying) {
		return;
	}

	Animation explosionAnim = {0,6,0,30,false,false};
	sprite.setAnimation(explosionAnim);
	sprite.scale({1.5f,1.5f});
	isDying = true;
}

Projectile::Projectile(sf::Vector2<float> og, float angle)
{
	origin = og;
	sprite.setPosition(og);
	firingAngle = angle;
}

Projectile::~Projectile()
{
}

void Projectile::update(float dt) {
	float x = std::cosf(firingAngle) * speed * dt;
	float y = std::sinf(firingAngle) * speed * dt;
	if (!isDying) {
		sprite.move({ x, y });
	}
	sprite.update(dt);


	//Check death state
	if (isDying && !sprite.getAnimated()) {
		//Then death animation has finished
		isDead = true;
	}

	float rng = maxRange;
	rng > range ? rng = range : rng = rng;
	if (utl::dist(origin.x,origin.y,sprite.getPosition().x,sprite.getPosition().y) >= rng) {
		this->playExplosion();
	}
}

void Projectile::draw(sf::RenderWindow& win) {
	sprite.draw(win);
}

void Projectile::setTexture(sf::Texture* texture)
{
	sprite.setTexture(texture);
	sprite.setFrameSize(32,32);
	sprite.setFrame(0);
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
}

//#####################################################################################################################
//		PROJECTILE MANAGER
//#####################################################################################################################

ProjectileManager::ProjectileManager()
{
}

ProjectileManager::~ProjectileManager()
{
	for (auto p : projectiles) {
		delete p;
	}
}

void ProjectileManager::update(float dt) {
	for (auto it = projectiles.begin(); it != projectiles.end();)
	{

		if ((*it)->getDead()) {
			delete (*it);
			it = projectiles.erase(it);
		}
		else {
			(*it)->update(dt);
			++it;
		}
	}
}

void ProjectileManager::draw(sf::RenderWindow& win) {
	for (auto p : projectiles) {
		p->draw(win);
	}
}

