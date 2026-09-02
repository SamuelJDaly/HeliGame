#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

#include "TextureRegistry.h"
#include "Spritesheet.h"

inline float radToDeg(float rad) {
	float pi = 3.14159f;
	return (rad * 180.f) / pi;
}

inline float degToRad(float deg) {
	float pi = 3.14159f;
	return (deg * pi) / 180.f;
}

inline float dist(sf::Vector2<float> a, sf::Vector2<float> b) {
	return std::sqrtf(std::pow(b.x - a.x, 2) + std::pow(b.y - a.y, 2));
}

class Player
{
private:
	//## Data
	Spritesheet sprite;
	
	//Movement
	int baseMp = 3; //Move pts
	int mpLeft = 3;
	std::vector<sf::Vector2<float>> path;
	float pathingSpeed = 100.f;
	bool doMovement = false;
	bool doDrawPath = true;
	bool isMoving = false;

	int baseHp = 100;
	int hpLeft = 100;

	//## Util
	void init(TextureRegistry &texReg);
	void drawPath(sf::RenderWindow &win);
	inline void constrainHp() { if (hpLeft > baseHp) { hpLeft = baseHp; } }

public:
	//## Constructor and Destructor
	Player(TextureRegistry &texReg);
	~Player();


	//## Primary Functions
	void update(float dt);
	void draw(sf::RenderWindow &window);

	void endTurn();
	void beginTurn();

	void pathTo(std::vector<sf::Vector2<float>> newPath);

	inline void setPos(sf::Vector2<float> newPos) { sprite.setPosition(newPos); }
	inline sf::Vector2<float> getPos() { return sprite.getPosition(); }
	inline void setPath(std::vector<sf::Vector2<float>> newPath) { path = newPath; }
	inline void setSize(sf::Vector2<float> newSize) { sprite.setSize(newSize); }
	inline bool getMoving() { return isMoving; }
	
	
	inline void setBaseHp(int newBaseHp) { baseHp = newBaseHp; this->constrainHp(); }
	inline int getBaseHp() { return baseHp; }
	inline void setHpLeft(int newHpLeft) { hpLeft = newHpLeft; this->constrainHp(); }
	inline int getHpLeft() { return hpLeft; }
	inline void modHpLeft(int amt) { hpLeft += amt; this->constrainHp(); }

};

