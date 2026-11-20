#pragma once
#include <vector>
#include <stack>
#include <unordered_map>
#include <algorithm>
#include <SFML/Graphics.hpp>

#include "imgui.h"
#include "imgui-SFML.h"
#include "TextureRegistry.h"
#include "AudioManager.h"
#include "Spritesheet.h"
#include "Tilemap.h"


/*
Project: Helicopter Game, Gamestate System
Created: 04 AUG 2026
Updated: 20 NOV 2026

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
	AudioManager* audioManager = nullptr;
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

	Gamestate(TextureRegistry* textureRegistry, AudioManager* audioManager, sf::RenderWindow* window) { texReg = textureRegistry; this->audioManager = audioManager;  win = window; };
	
	//Virtual Functions
	virtual void update(float dt) = 0;
	virtual void poll(sf::RenderWindow& win, std::optional<sf::Event> event) = 0;
	virtual void draw(sf::RenderWindow& win) = 0;
};