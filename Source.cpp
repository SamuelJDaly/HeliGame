#include "Engine.h"


/*
Project: Utility AI Practice, Driver code
Created: 04 AUG 2026
Updated: 04 AUG 2026

Description:
	This file contains the driver code for the engine.

*/

int main(int argc, char** argv) {
	Engine engine;


	while (engine.getRunning()) {
		engine.poll();
		engine.update();
		engine.draw();
	}

	return 0;
}