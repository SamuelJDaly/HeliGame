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
	sf::SoundBuffer* lookupSound(std::string name);

public:
	//## Constructor and Destructor
	AudioManager();
	~AudioManager();

	//## Primary Functions
	void registerSound(std::string name, std::string filename);

	void playSound(std::string name, float volume = 100.f);
	void update();
};

