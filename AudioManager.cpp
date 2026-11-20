#include "AudioManager.h"

AudioManager::AudioManager()
{

}

AudioManager::~AudioManager()
{

}


void AudioManager::registerSound(std::string name, std::string filename)
{
	//Check if sound can be loaded
	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile(filename))
	{
		std::cout << "Could not load sound: " << filename << std::endl;
		return;
	}

	//Add buffer to registry
	soundRegistry.insert({name, buffer});
}

sf::SoundBuffer* AudioManager::lookupSound(std::string name)
{
	auto it = soundRegistry.find(name);

	if (it == soundRegistry.end())
	{
		std::cout << "Sound not found: [" << name << "]" << std::endl;
		return nullptr;
	}
	else
	{
		return &soundRegistry.at(name);
	}


	return nullptr;
}

void AudioManager::playSound(std::string name)
{
	if (numSounds >= 255)
	{
		std::cout << "Could not play sound: max number of sounds reached..." << std::endl;
		return;
	}

	sf::SoundBuffer* buffer = this->lookupSound(name);

	if (!buffer)
	{
		std::cout << "Sound not found in registry: " << name << std::endl;
		return;
	}

	sounds.push(sf::Sound(*buffer));
	sounds.back().play();
	numSounds++;
}

void AudioManager::update()
{
	while (!sounds.empty() && sounds.front().getStatus() == sf::SoundSource::Status::Stopped)
	{
		sounds.pop();
		numSounds--;
	}
}