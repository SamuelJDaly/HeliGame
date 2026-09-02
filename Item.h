#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Spritesheet.h"

struct Effect {
	int score = 0;
	int hpRestore = 0;
};

class Item
{
private:
	//## Data
	Spritesheet sprite;
	Effect effect;

	//## Util


public:
	//## Constructor and Destructor
	Item();
	~Item();

	//## Primary Functions
	void draw(sf::RenderWindow &window);
	void setSize(sf::Vector2<float> size);
	void setTexture(sf::Texture* newTexture);

	inline Effect getEffect() { return effect; }
	inline void setEffect(Effect newEffect) { effect = newEffect; }
	inline void setPosition(sf::Vector2<float> newPos) { sprite.setPosition(newPos); }
	inline sf::Vector2<float> getPosition() { return sprite.getPosition(); }
};

