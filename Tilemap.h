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
PROJECT: Heli Game
CREATED:4 OCT 2026
AUTHOR: Sam Daly

NOTES:
	-Make sure spritesheet has a working copy constructor
	-Adjust tile copy constructor and related functions to account for added position attribute
*/


namespace tmap
{


struct Tile
{
	int type = -1;
	int sheet = -1;
	bool isSolid = false;

	sf::Vector2i coords = { -1,-1 };

	Spritesheet sprite;

	Tile(Tile* cpy) { type = cpy->type; sheet = cpy->sheet; isSolid = cpy->isSolid; sprite = Spritesheet(cpy->sprite); coords = cpy->coords; } //Need to make sure this is kosher...
	Tile() {};
};


class TileLayer
{
private:
	//## Data
	std::vector<std::vector<Tile*>> tiles;
	sf::Vector2<float> offset = { 0.f,0.f };
	int tileSize = 32; //Should this be stored here? its dupliation of data...

	//## Util
	bool validIdx(int x, int y);
	

public:
	//## Constructor and Destructor
	TileLayer(int width, int height, int tilesize);
	~TileLayer();

	//## Primary Functions
	Tile getTileAt(int x, int y);
	void setTileAt(int x, int y, const Tile& data);
	void setTypeAt(int x, int y, int type, TextureRegistry &texReg, std::vector<std::pair<std::string, int>> tilesets, int tilesize);
	void setSheetAt(int x, int y, int sheet, TextureRegistry& texReg, std::vector<std::pair<std::string, int>> tilesets, int tilesize);
	void setSolidAt(int x, int y, int state);
	void setTileDataAt(int x, int y, int type, int sheet, int isSolid, TextureRegistry& texReg, std::vector<std::pair<std::string, int>> tilesets, int tilesize);
	void replaceTileAt(int x, int y, Tile* tile);
	

	std::optional<sf::Rect<float>> intersectionAt(int x, int y, sf::FloatRect rect);
	bool containsAt(int x, int y, sf::Vector2<float> pos);
	
	void setOffset(sf::Vector2<float> newOffset);
	inline sf::Vector2<float> getOffset() { return offset; }
	void move(sf::Vector2<float> amt);

	void draw(sf::RenderWindow &win);
};


class Tilemap
{
private:
	//## Data
	sf::Vector2<float> offset;
	std::vector<TileLayer*> layers;
	sf::Vector3<int> dimensions = {0, 0, 0};
	TextureRegistry* texReg = nullptr;
	std::vector<std::pair<std::string, int>> tilesets;
	int tileSize = 32;

	//## Util
	void clearLayers();
	int readLayer(std::ifstream &inFile);
	int writeLayer(std::ofstream &outFile, int layer);

public:
	//## Constructor and Destructor
	Tilemap(int width, int height, int layers, int tileSize, TextureRegistry* textureRegistry);
	~Tilemap();

	//## Primary Functions
	int loadFromFile(std::string filename);
	int writeToFile(std::string filename);

	void poll(sf::RenderWindow &win, std::optional<sf::Event> event);
	void update(float dt);
	void draw(sf::RenderWindow& win);
};


}

