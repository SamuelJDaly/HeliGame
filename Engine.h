#pragma once
#include <iostream>
#include <random>
#include <time.h>
#include <SFML/Graphics.hpp>

#include "TextureRegistry.h"
#include "AudioManager.h"
#include "Gamestate.h"
#include "State_Heli.h"
#include "EditorState.h"

/*
Project: Helicopter Game, Engine
Created: 04 AUG 2026
Updated: 20 NOV 2026

Description:
	This file contains the function and class definitions for the Engine, which has the following responsibilities:
		1. Resource Management
			The Engine manages resources used by the various game states like textures and audio.
			It uses the texture registry system to ensure that textures are only loaded once.

			Because the sfml audio system is similar to its texture system, the Audio Manager does almost the exact same thing for sounds.
			The difference being that while sprites tend to stick around and are managed outside the texture registry, sfx have no need
			to exist outside the audio manager, and have a short lifecycle. A texture is to a sprite what an audio buffer is to a sound.

		2. State Management
			The Engine is also a state manager, controlling what state is active, and calling the various game loop
			functions for that state (poll, update, draw).


		3. Window Management
			The Engine also initializes the window and manages its lifecycle.

		4. Context management
			The Engine is responsible for managing contexts for outside libraries that require one. For now this just includes DearIMGUI, but
			should BOX2D be integrated, its context would also be initialized, updated, and disposed by the Engine class (just as an example).

*/

class Engine
{
private:
	//Data
	sf::RenderWindow* win;
	sf::Clock mainClock;
	bool isRunning = true;
	bool isMouseGrabbed = false;
	TextureRegistry texReg;
	AudioManager audioManager;
	sf::Texture defaultTexture;
	std::string textureManifestPath = "resource/tex/manifest.txt";

	//Debug info
	bool doShowDebugInfo = true;
	sf::Font fnt;
	sf::Text fpsText = sf::Text(fnt);
	int fps = 0;
	float frameTimer = 0.f;
	int frameCount = 0;

	float deltaTime = 0;

	Gamestate* currState;

	//Util
	void initWindow();
	void initTextures();
	void initAudio();
	void initState();

public:
	//Constructor and Destructor
	Engine();
	~Engine();

	//Primary Functions
	void poll();
	void update();
	void draw();

	void writeTextures();

	bool getRunning();
};

