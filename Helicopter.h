#pragma once
#include <iostream>
#include "SFML/Graphics.hpp"
#include "Spritesheet.h"
#include "Projectile.h"
#include "Utility.h"


class Helicopter
{
private:
	//## Data
	sf::View* map_view;
	Spritesheet sprite;
	unsigned int size = 32;
	float speed = 100.f;
	sf::Vector2<float> lastPos = {0,0};
	sf::Vector2<float> lastOffset = { 0,0 };

	int kills = 0;

	//## Controller
	sf::Vector2<float> vel = {0.f,0.f};
	sf::Vector2<float> mom = {0.f,0.f};
	sf::Vector2<float> maxMom = { 10000.f,10000.f };
	float drag = 0.999f;
	float mass = 100.f;
	float delta = 60.f;

	ProjectileManager projectileMgr;

	//Weapons
	int weaponSel = 0;
	int numWeapons = 2;

	sf::Texture* rocketTexture;
	sf::Vector2f rocketSize = { 16,16 };
	int rocketSpreadPercent = 10; //Find a better way to handle this that allows for finer control (ie floats)
	float rocketSpeed = 500.f;
	float rocketMaxRange = 500.f;
	int rocketDmg = 10;
	bool rocketSide = 0;
	int rocketClipSize = 8;
	int rocketClipCurr = 8;
	float rocketReloadTimer = 0.f;
	float rocketReloadInterval = 5.f;
	float rocketFireTimer = 0.f;
	float rocketFireInterval = 0.1f;
	bool canRocketFire = true;
	bool isRocketReloading = false;
	bool isRocketWatingToFire = false;

	sf::Texture* bulletTexture;
	sf::Vector2f bulletSize = { 16,16 };
	int cannonSpreadPercent = 5; //Find a better way to handle this that allows for finer control (ie floats)
	float cannonSpeed = 500.f;
	float cannonMaxRange = 700.f;
	int cannonDmg = 1;
	float cannonFireTimer = 0.f;
	float cannonFireInterval = 0.05f;
	bool canCannonFire = true;
	bool isCannonWatingToFire = false;
	
	//Debug
	sf::Font font;
	sf::Color debugTextFgCol = sf::Color::Green;
	sf::Color debugTextBgCol = sf::Color::Black;
	float debugTextSize = 15.f;
	sf::Text momText = sf::Text(font);
	sf::Text velText = sf::Text(font);
	sf::Text angleText = sf::Text(font);
	sf::Text posText = sf::Text(font);
	sf::Text weaponText = sf::Text(font);
	sf::Text killsText = sf::Text(font);

	sf::Vector2<float> mPos = {0.f,0.f};

	sf::RectangleShape textBgs[6];

	sf::CircleShape reloadIndTop;
	sf::CircleShape reloadIndBottom;
	float reloadIndScaleSpeed = 0.f;
	float reloadIndCurrScale = 1.f;


	//## Util
	void init();
	void drawDebug(sf::RenderWindow &win);
	void lookAtPoint(sf::Vector2<float> point);

	void fireWeapon(sf::Vector2<float> targetPos);

	void updateController(float dt, sf::RenderWindow& win);
	void updateBasic(float dt, sf::RenderWindow& win);
	
	
	void updateFireControl(float dt, sf::Vector2<float> mapViewMousePos);
	void updateRocketFireControl(float dt, sf::Vector2<float> mapViewMousePos);
	void updateCannonFireControl(float dt, sf::Vector2<float> mapViewMousePos);
	

public:
	//## Constructor and Destructor
	Helicopter();
	~Helicopter();

	//## Primary Functions
	void update(float dt, sf::RenderWindow& win);
	void poll(sf::RenderWindow& win, std::optional<sf::Event> event);
	void draw(sf::RenderWindow &win);

	void setTexture(sf::Texture* texture);
	
	inline void setPos(sf::Vector2<float> newPos) { sprite.setPosition(newPos); lastPos = newPos; }
	inline sf::Vector2<float> getPos() { return sprite.getPosition(); }
	inline sf::Vector2<float> getLastPos() { return lastPos; }
	inline void setAnimation(Animation anim) { sprite.setAnimation(anim); }
	inline void setMapView(sf::View* newView) { map_view = newView; projectileMgr.setView(*newView); }
	inline void setRocketTexture(sf::Texture* tex) { rocketTexture = tex; }
	inline void setBulletTexture (sf::Texture* tex) { bulletTexture = tex; }
	inline sf::Vector2<float> getLastOffset() { return lastOffset; }

	inline sf::Rect<float> getGlobalBounds() { return sprite.getGlobalBounds(); }

	inline ProjectileManager* getProjectileMgr() { return &projectileMgr; }

	inline void addKill(int amt = 1) { kills += amt; }

};

