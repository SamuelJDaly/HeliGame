#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <SFML/Graphics.hpp>

/*
//Created: 11 JAN 2025
//Updated: 13 APR 2026
//Author: Sam Daly

Description:
	Within the SFML framework loading textures is a relatively expensive operation, and storing duplicates can take up unneccesary memory.
	This file contains a class called the TextureRegistry, which has two main goals:
		1. Optimize texture use
		2. Make it more convenient to find a given texture

	The first goal is accomplished by eliminating duplicate texture objects, ie there is one object for each texture, and a pointer to it is passed
	to each object that needs that texture. Another way the first goal is accomplished is by loading each texture only as many times as needed
	(once in theory).

	The second goal is met by associating a texture with a string via a map. This may not be the most performance friendly method, but for the purposes
	of this simple engine will do just fine.

*/

//###################	UTIL	#######################################################
std::vector<std::string> split(std::string str, char delim = ' ');


//###################	TEXTURE HANDLER	###############################################
class TextureRegistry
{
private:
	//Data
	std::unordered_map<std::string, sf::Texture> textures;
	bool isVerbose = true;

	sf::Texture defaultTexture;

public:
	//Constructor and Destructor
	TextureRegistry();
	~TextureRegistry();

	//Primary functions
	sf::Texture* lookup(std::string key);
	void fLoad(std::string filepath);
	void addTexture(std::string key, std::string filepath);
	void setVerbose(bool state);

};

