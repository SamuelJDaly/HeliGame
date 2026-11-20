#pragma once
#include <iostream>
#include <queue>
#include <unordered_map>
#include <SFML/Audio.hpp>


class AudioManager
{
private:
	//## Data
	std::unordered_map<std::string, sf::SoundBuffer> soundRegistry;
	std::queue<sf::Sound> sounds;

	int numSounds = 0;

	bool soundLoaded = false;

	//## Util


public:
	//## Constructor and Destructor
	AudioManager();
	~AudioManager();

	//## Primary Functions
	void registerSound(std::string name, std::string filename);
	sf::SoundBuffer* lookupSound(std::string name);
	

	void playSound(std::string name);
	void update();
};

