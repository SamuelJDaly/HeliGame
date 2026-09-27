#pragma once
#include <iostream>
#include <vector>
#include "Spritesheet.h"
#include "Projectile.h"


class Enemy
{
private:
	//## Data
	Spritesheet sprite;

	int id = -1;

	bool isDead = false;
	bool isDying = false;
	bool isFlipped = false;
	bool isDebugDrawn = false;

	sf::FloatRect hitBox = { {0.f,0.f},{1.f,1.f} };
	sf::Vector2<float> scaleFactor = {1.f,1.f};

	//Stats
	int baseHp = 10;
	int currHp = 10;
	float speed = 100.f;

	//Ai
	sf::Vector2<float> targetPos = {0,0};
	bool isTargetReached = true;
	int wanderRange = 100;
	float wanderTimer = 0.f;
	float wanderIntervalBase = 3.f; //How long to idle without random adjustment
	float wanderIntervalCurr = 0.f; //Actual wander interval

	//## Util
	void updateBasic(float dt);
	void die();
	void drawDebug(sf::RenderWindow &win);

public:
	//## Constructors and Destructor
	Enemy(int id);
	~Enemy();

	//## Primary Functions
	void update(float dt);
	void draw(sf::RenderWindow &win);

	void setTexture(sf::Texture* texture, bool resetHibox = true);
	void setPosition(sf::Vector2<float> pos);

	inline int getID() { return id; }

	inline bool getDead() { return isDead; }
	inline void modCurrHP(int amt) { currHp += amt; }
	inline void setCurrHP(int amt) { currHp = amt; }
	inline void modBaseHP(int amt) { baseHp += amt; }
	inline void setBaseHP(int amt) { baseHp = amt; }

	inline int getCurrHp() { return currHp; }

	void setScale(sf::Vector2<float> factor);
	
	inline sf::Rect<float> getGlobalBounds() { return sprite.getGlobalBounds(); }

	inline void setHitbox(sf::FloatRect newHitBox) { hitBox = newHitBox; }
	sf::FloatRect getHitbox();

	inline void SetIsDebugDrawn(bool state) { isDebugDrawn = state; }
};

