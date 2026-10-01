#include "Engine.h"


/*
Project: Helicopter Game, Driver code
Created: 04 AUG 2026
Updated: 19 SEP 2026

Description:
	This file contains the entry point for the application and drives the main engine functions.

*/

int main(int argc, char** argv) {
	Engine engine;


	while (engine.getRunning()) {
		engine.poll();
		engine.update();
		engine.draw();
	}

	engine.writeTextures();

	return 0;
}