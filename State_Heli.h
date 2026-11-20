#pragma once
#include "Gamestate.h"
#include "Tilemap.h"
#include "Helicopter.h"
#include "Enemy.h"
#include "Collision.h"

/*
TODO:
 - Check that projectile manager deletes remaining projectiles upon destruction: NOT DONE
 - Check that enemies list deletes remaining enemies upon destruction: NOT DONE
 - Make weapons shoot a little past mouse, find myself shooting short a lot
*/

class State_Heli : public Gamestate
{
private:
	//## Data
	//Camera
	sf::View view_map;
	sf::Vector2<float> cameraOffset = { 0.f,0.f };
	float panSpeed = 850.f;
	float maxZoom = 1.5f;
	float minZoom = 0.1f;
	float zoomSpeed = .06f;
	float zoomStep = .1f;
	float currZoom = 1.f;

	//Game elements
	tmap::Tilemap* map;

	Helicopter* player;
	ProjectileManager projectileMgr;
	std::vector<Explosion*> explosions;

	int enemyCount = 0;
	std::vector<Enemy*> enemies;

	//## Util
	void moveCamera(sf::Vector2<float> offset);
	void updateCamera(float dt);
	void updateEnemies(float dt);
	void init();
	void updateTest(float dt); //Scenario stuff
	void spawnRandomEnemies(int amt);

	void updateCollision(float dt); //Collision stuff
	void updateProjectiles(float dt);
	void updateExplosions(float dt); //Explosion stuff
public:
	//## Constructor and Destructor
	State_Heli(TextureRegistry* textureRegistry, AudioManager* audioManager, sf::RenderWindow* window);
	~State_Heli();

	//## Primary Functions
	void update(float dt);
	void poll(sf::RenderWindow& win, std::optional<sf::Event> event);
	void draw(sf::RenderWindow& win);
};

