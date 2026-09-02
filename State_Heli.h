#pragma once

#include "Gamestate.h"
#include "Map.h"
#include "Helicopter.h"
#include "Enemy.h"

/*
REMEMBER:
 1. Check that projectile manager deletes remaining projectiles upon destruction: NOT DONE
 2. Check that enemies list deletes remaining enemies upon destruction: NOT DONE
*/

class State_Heli : public Gamestate
{
private:
	//## Data
	sf::View view_map;
	sf::Vector2<float> cameraOffset = { 0.f,0.f };
	float panSpeed = 850.f;
	float maxZoom = 1.5f;
	float minZoom = 0.1f;
	float zoomSpeed = .06f;
	float zoomStep = .1f;
	float currZoom = 1.f;

	Map* map;

	Helicopter* player;

	int enemyCount = 0;
	std::vector<Enemy*> enemies;

	//## Util
	void moveCamera(sf::Vector2<float> offset);
	void updateCamera(float dt);
	void updateEnemies(float dt);
	void initTest();
	void updateTest(float dt);
	void spawnRandomEnemies(int amt);

	void updateCollision(float dt);
public:
	//## Constructor and Destructor
	State_Heli(TextureRegistry* textureRegistry, sf::RenderWindow* window);
	~State_Heli();

	//## Primary Functions
	void update(float dt);
	void poll(sf::RenderWindow& win, std::optional<sf::Event> event);
	void draw(sf::RenderWindow& win);
};

