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
	float aoeRadius = 1.f;

	int team = -1;
	int damage = 0;
	float speed = 300.f;
	float firingAngle = 0.f; //Radians
	float range = 100.f;
	float maxRange = 100.f;

	sf::Vector2<float> origin = { 0.f,0.f };

	bool isDying = false;
	bool isDead = false;
	bool doesExplode = false;
	bool wantsToExplode = false;
	bool hasExploded = false;

	//## Util
	void playExplosion();

public:
	//## Constructor and Destructor
	Projectile(sf::Vector2<float> og, float angle);
	~Projectile();

	//## Primary Functions
	void update(float dt);
	void draw(sf::RenderWindow &win);

	inline void setAoeRadius(float radius) { aoeRadius = radius; }
	inline float getAoeRadius() { return aoeRadius; }

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
	inline void detonate() { this->playExplosion(); this->wantsToExplode = true; }
	inline bool getDead() { return isDead; }
	inline bool getDying() { return isDying; }
	inline void setTeam(int newTeam) { team = newTeam; }
	inline int getTeam() { return team; }

	inline sf::Vector2<float> getPos() { return sprite.getPosition(); }

	inline void setDoesExplode(bool state) { doesExplode = state; }
	inline bool getDoesExplode() { return doesExplode; }
	inline bool getWantsToExplode() { return wantsToExplode; }
	inline bool getHasExploded() { return hasExploded; }
	inline void setHasExploded(bool state) { hasExploded = state; }

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

class Explosion
{
private:
	//## Data
	Spritesheet sprite;
	float radius = 1.f;
	int dmg = 1;
	int team = -1;

	Animation explosionAnim;

	//## Util

public:
	//## Constructor and Destructor
	Explosion();
	~Explosion();

	//## Primary Functions
	void playExplosion();
	void setTexture(sf::Texture* texture, unsigned int frameSize = 32);
	void setAnimation(Animation anim);
	void setPos(sf::Vector2<float> pos);

	inline void setRadius(float newRadius) { radius = newRadius; }
	inline float getRadius() { return radius; }
	inline void setDmg(int newDmg) { dmg = newDmg; }
	inline int getDmg() { return dmg; }
	inline bool getDone() { return sprite.getAnimationDone(); }
	inline void setSize(sf::Vector2<float> size) { sprite.setSize(size); sprite.setOrigin(sprite.getLocalBounds().getCenter()); }
	

	void update(float dt);
	void draw(sf::RenderWindow &win);
};

