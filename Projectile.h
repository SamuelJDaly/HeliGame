#pragma once
#include <iostream>
#include <vector>
#include "Spritesheet.h"
#include "Utility.h"

class Projectile
{
private:
	//## Data
	Spritesheet sprite;

	int team = -1;
	int damage = 0;
	float speed = 300.f;
	float firingAngle = 0.f; //Radians
	float range = 100.f;
	float maxRange = 100.f;

	sf::Vector2<float> origin = { 0.f,0.f };

	bool isDying = false;
	bool isDead = false;

	//## Util
	void playExplosion();

public:
	//## Constructor and Destructor
	Projectile(sf::Vector2<float> og, float angle);
	~Projectile();

	//## Primary Functions
	void update(float dt);
	void draw(sf::RenderWindow &win);

	void setTexture(sf::Texture* texture);
	inline void setSize(sf::Vector2<float> newSize) { sprite.setSize(newSize); }

	inline void setSpeed(float newSpeed) { speed = newSpeed; }
	
	inline void setDamage(int newDamage) { damage = newDamage; }
	inline int getDamage() { return damage; }

	inline void setRange(float newRange) { range = newRange; }
	inline void setMaxRange(float newMaxRange) { maxRange = newMaxRange; }
	inline float getRange() { return range; }

	inline void setRotation(sf::Angle rotation) { sprite.setRotation(rotation); }

	inline void kill() { this->isDead = true; }
	inline void detonate() { this->playExplosion(); }
	inline bool getDead() { return isDead; }
	inline bool getDying() { return isDying; }
	inline void setTeam(int newTeam) { team = newTeam; }
	inline int getTeam() { return team; }

	inline sf::Rect<float> getGlobalBounds() { return sprite.getGlobalBounds(); }

	inline void setPosition(sf::Vector2<float> pos) { sprite.setPosition(pos); }
};


class ProjectileManager {
private:
	//## Data
	std::vector<Projectile*> projectiles;
	sf::View view;

	//## Util

public:
	//## Constructor and Destructor
	ProjectileManager();
	~ProjectileManager();

	//## Primary Functions
	void update(float dt);
	void draw(sf::RenderWindow& win);

	inline void setView(sf::View newView) { view = newView; }
	
	inline void addProjectile(Projectile* p) { projectiles.push_back(p); }
	inline std::vector<Projectile*> getProjectiles() { return projectiles; }
};

