#pragma once
#include <iostream>
#include <random>
#include <time.h>
#include <SFML/Graphics.hpp>

#include "TextureRegistry.h"
#include "Gamestate.h"
#include "State_Heli.h"
#include "EditorState.h"

/*
Project: Helicopter Game, Engine
Created: 04 AUG 2026
Updated: 19 SEP 2026

Description:
	This file contains the function and class definitions for the Engine, which has the following responsibilities:
		1. Resource Management
			The Engine manages resources used by the various game states like textures and audio(not yet implemented).
			It uses the texture registry system to ensure that textures are only loaded once.

		2. State Management
			The Engine is also a state manager, controlling what state is active, and calling the various game loop
			functions for that state (poll, update, draw).


		3. Window Management
			The Engine also initializes the window and manages its lifecycle.

*/

class Engine
{
private:
	//Data
	sf::RenderWindow* win;
	sf::Clock mainClock;
	bool isRunning = true;
	bool isMouseGrabbed = false;
	TextureRegistry* texReg;
	sf::Texture defaultTexture;

	float deltaTime = 0;

	Gamestate* currState;

	//Util
	void initWindow();
	void initTextures();
	void initState();

public:
	//Constructor and Destructor
	Engine();
	~Engine();

	//Primary Functions
	void poll();
	void update();
	void draw();

	bool getRunning();
};

