#pragma once
#include <vector>
#include <stack>
#include <unordered_map>
#include <algorithm>
#include <SFML/Graphics.hpp>

#include "imgui.h"
#include "imgui-SFML.h"
#include "TextureRegistry.h"
#include "Spritesheet.h"

#include "Map.h"
#include "Player.h"
#include "Item.h"

/*
Project: Helicopter Game, Gamestate System
Created: 04 AUG 2026
Updated: 20 SEP 2026

Description:
	This file contains the Gamestate System. This is what actually ties the various systems together and manages their interactions.
	These states have some common program loop functions that are called by the engine class once per frame:
		Poll:
			This is where less time dependent event logic happens, like window closures, button clicks, etc...

		Update:
			This is where per-frame logic happens. Events that need to happen once per frame for smooth movement, etc are called here. This includes things
			like collision, movement, etc...

		Draw:
			This is where any draw calls happen.
*/

//##########################	STATE ENUMS	#########################################
enum class En_Gamestate {
	MENU, GAME, EDITOR, TEST, END
};

//##########################	BASE CLASS	#########################################
class Gamestate
{
protected:
	//Common Data
	TextureRegistry* texReg = nullptr;
	sf::RenderWindow* win;
	bool isFinished = false;
	En_Gamestate nextState = En_Gamestate::END;
	sf::Color clearColor = sf::Color::Black;

public:
	//Common Functions
	bool getFinished();
	En_Gamestate getNextState();

	inline sf::Color getClearColor() { return clearColor; }
	inline void setClearColor(sf::Color newClearColor) { clearColor = newClearColor; }

	Gamestate(TextureRegistry* textureRegistry, sf::RenderWindow* window) { texReg = textureRegistry;  win = window; };
	
	//Virtual Functions
	virtual void update(float dt) = 0;
	virtual void poll(sf::RenderWindow& win, std::optional<sf::Event> event) = 0;
	virtual void draw(sf::RenderWindow& win) = 0;
};

//##########################	GAME STATE	#########################################
class State_Game : public Gamestate {
private:
	//## Data
	sf::View view_map;
	sf::Vector2<float> cameraOffset = {0.f,0.f};
	float panSpeed = 850.f;
	float maxZoom = 1.5f;
	float minZoom = 0.1f;
	float zoomSpeed = .06f;
	float zoomStep = .1f;
	float currZoom = 1.f;
	

	Player* player;
	Map* map;
	std::vector<Item*> items;

	int score = 0;
	sf::Font font;
	sf::Text scoreText = sf::Text(font);
	sf::Text hpText = sf::Text(font);
	

	//## Util
	void init();

	void moveCamera(sf::Vector2<float> offset);
	void updateCamera(float dt);
	void updateScore();
	void updateHPText();
	void updateItemCollision();

	void updatePlayerInput();
	void pollPlayerInput(sf::RenderWindow& win, std::optional<sf::Event> event);

public:
	//Constructors and Destructors
	State_Game(TextureRegistry* textureRegistry, sf::RenderWindow* window);
	~State_Game();

	//Primary Functions
	void endTurn();
	void beginTurn();
	void update(float dt);
	void poll(sf::RenderWindow& win, std::optional<sf::Event> event);
	void draw(sf::RenderWindow& win);

};