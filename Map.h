#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <SFML/Graphics.hpp>

#include "TextureRegistry.h"
#include "Spritesheet.h"
#include "Utility.h"

/*
Project: Helicopter Game, Map System
Created: 04 AUG 2026
Updated: 19 SEP 2026

Description:
	The Map system allows a grid of tiles to be manipulated and displayed. It is intended to be adaptable to a variety of game styles, and is
	comprised of the following components

	1. Tile
		The Tile struct contains basic tile data including sprite/texture


	2. Map
		
*/

struct Tile {
	sf::Vector2<unsigned int> pos = { 0,0 };
	Spritesheet sprite;
	sf::Angle rotation = sf::degrees(0.f);
	int sheet = -1;
	int type = -1;
	float moveCost = 1.f;

	bool isPassable = true;
	


	bool isHighlight = false;

	inline bool contains(sf::Vector2<float> pos) {
		if (sprite.contains(pos)) {
			return true;
		}

		return false;
	}

	inline void Poll(sf::RenderWindow& win, std::optional<sf::Event> event) {

	}


	inline void draw(sf::RenderWindow& win) {
		sprite.draw(win);
	}
};

inline std::ostream& operator<<(std::ostream& os, Tile& t) {
	os << "[ " << t.sheet << t.type << " " << t.isPassable << " " << t.moveCost << " " << t.sprite.getRotation().asDegrees() << " ]";
	return os;
}




class Map
{
private:
	//Data
	sf::Vector2<unsigned int> dimensions = { 0,0 };
	int numTilesets = 1;
	std::vector<std::pair<std::string, int>> tilesets;
	std::vector<std::vector<Tile*>*> tiles;
	TextureRegistry* texReg;
	//std::vector<sf::Vertex> grid;
	sf::Vector2<float> offset = { 0,0 };
	int tileSize = 32;
	sf::View* view;
	bool drawGrid = true;
	sf::Color gridColor = sf::Color::Green;
	std::vector<sf::Vertex> grid;
	sf::Texture* fogTexture;


	//Util
	void align();
	void generateGrid();

public:
	//Constructors and Destructor
	Map(TextureRegistry* textureRegistry);
	~Map();

	//Primary Functions
	inline void setGridColor(sf::Color col) { gridColor = col; this->generateGrid(); }

	std::vector<sf::Vector2<float>> pathfind(sf::Vector2<int> origin, sf::Vector2<int> target);
	void setOffset(sf::Vector2<float> newOffset);
	sf::Vector2<int> posToTileIdx(sf::Vector2<float> pos);
	sf::Vector2<float> tileIdxToPos(sf::Vector2<int> idx);

	sf::Vector2<float> getSize();
	inline int getTileSize() { return tileSize; };

	int loadFromFile(std::string filename);
	sf::Vector2<float> getOffset();

	Tile* tileAtIdx(size_t x, size_t y);
	Tile* tileAtIdx(std::pair<int, int> idx);

	//float mnhtnDist(sf::Vector2<int> a, sf::Vector2<int> b);
	int mnhtnDist(sf::Vector2<float> a, sf::Vector2<float> b);


	std::vector<sf::Vector2<int>> tileIdxInRange(int range, sf::Vector2<int> og, bool includeOG = false);

	inline void setVeiw(sf::View* newView) { this->view = newView; }
	inline sf::View* getView() { return this->view; }

	inline void setGridDraw(bool state) { drawGrid = state; }

	void Poll(sf::RenderWindow& win, std::optional<sf::Event> event);
	void Update();
	void Draw(sf::RenderWindow& win);
};

