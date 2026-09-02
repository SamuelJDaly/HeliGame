#pragma once
#include <iostream>
#include <random>
#include <time.h>
#include <SFML/Graphics.hpp>

#include "TextureRegistry.h"
#include "Gamestate.h"
#include "State_Heli.h"

class Engine
{
private:
	//Data
	sf::RenderWindow* win;
	sf::Clock mainClock;
	bool isRunning = true;
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

