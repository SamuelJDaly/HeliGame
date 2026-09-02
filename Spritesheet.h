#pragma once
#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>


//######### Animation
struct Animation {
	unsigned int startFrame = -1;
	unsigned int endFrame = -1;
	unsigned int currFrame = 0;
	float fps = 1;
	bool isDone = false;
	bool repeat = false;
	float repeatDur = -1; //-1 = inf
	float repeatLeft = -1;

	inline void adv() {
		//Advance
		currFrame++;

		if (repeat && repeatDur >= 0) {
			repeatLeft -= 1 / fps;
		}

		//End or continue
		if (currFrame >= endFrame) {
			currFrame = startFrame;

			if (repeat && repeatLeft <= 0.f) {
				isDone = true;
			}

			if (!repeat) {
				isDone = true;
			}
		}
	}
};


//######## Spritesheet
class Spritesheet
{
private:
	//## Data
	sf::Texture defaultTexture;
	sf::Sprite sprite = sf::Sprite(defaultTexture);

	sf::Vector2<unsigned int> dimensions = { 1,1 };
	sf::Texture* texture = nullptr;
	std::string texturePath = "";
	sf::Vector2<unsigned int> frameSize = { 1,1 };
	int numFrames = 1;
	int currFrameIdx = 0;

	//Animation
	bool isAnimated = false;
	Animation currAnimation;
	float animTimer = 0.f;

	//Debug
	bool areBoundsDrawn = false;

	//##Util
	sf::Rect<int> calculateRect();
	bool checkAnimation(Animation anm);
	void drawBounds(sf::RenderWindow& win);

public:
	//Constructors and Destructor
	Spritesheet();
	~Spritesheet();

	//Primary Functions
	void setFrame(int idx);
	void advFrame(int amt);

	void setPosition(sf::Vector2<float> pos);
	void move(sf::Vector2<float> offset);
	void setScale(sf::Vector2<float> scale);
	void scale(sf::Vector2<float> factor);
	void setSize(sf::Vector2<float> size);
	void setRotation(sf::Angle angle);
	void rotate(float degrees);
	void setOrigin(sf::Vector2<float> origin);
	void setColor(sf::Color color);
	void setAnimation(Animation animation);

	inline bool getAnimated() { return isAnimated; }
	bool contains(sf::Vector2<float> pos);

	sf::Vector2<float> getPosition();
	sf::Vector2<float> getScale();
	sf::Angle getRotation();
	sf::Vector2<float> getOrigin();
	sf::Rect<float> getLocalBounds();
	sf::Rect<float> getGlobalBounds();
	sf::Texture* getTexture();
	sf::Rect<int> getTextureRect();

	void setDimensions(sf::Vector2<unsigned int> newDimensions);
	void setTexture(sf::Texture* newTexture);
	void setFrameSize(unsigned int sizeX, unsigned int sizeY);
	void draw(sf::RenderWindow& win);
	void update(float dt);

	inline void setDrawBounds(bool state) { areBoundsDrawn = state; }

	//int writeToFile(std::string filename);
	//int readFromFile(std::string filename);
};

