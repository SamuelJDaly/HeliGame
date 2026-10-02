#include "Spritesheet.h"
#include "Gamestate.h"

sf::Rect<int> Spritesheet::calculateRect()
{
	int offsetX = (currFrameIdx % dimensions.x) * frameSize.x;
	int offsetY = (int)std::floor(currFrameIdx / dimensions.x) * frameSize.y;
	return { {offsetX,offsetY},{(int)frameSize.x, (int)frameSize.y} };
}

bool Spritesheet::checkAnimation(Animation anm)
{
	if (anm.startFrame < 0 || anm.startFrame >= numFrames) {
		return false;
	}

	if (anm.endFrame < 0 || anm.endFrame >= numFrames) {
		return false;
	}

	if (anm.startFrame > anm.endFrame) {
		return false;
	}

	return true;
}

void Spritesheet::drawBounds(sf::RenderWindow& win)
{
	sf::RectangleShape r;
	r.setSize({ sprite.getGlobalBounds().size.x, sprite.getGlobalBounds().size.y });
	//r.setOrigin(sprite.getOrigin());
	//r.setPosition(sprite.getPosition());
	r.setPosition(sprite.getGlobalBounds().position);
	

	r.setFillColor(sf::Color::Transparent);
	r.setOutlineColor(sf::Color::Green);
	r.setOutlineThickness(1.f);

	win.draw(r);
}

Spritesheet::Spritesheet()
{
}

Spritesheet::~Spritesheet()
{
}

void Spritesheet::setFrame(int idx) {
	if (idx < 0 || idx >= numFrames) {
		std::cout << "Texture frame not set: Invalid idx: " << idx << std::endl;
		return;
	}

	currFrameIdx = idx;
	sprite.setTextureRect(calculateRect());
}

void Spritesheet::advFrame(int amt) {
	currFrameIdx += amt;

	while (currFrameIdx >= numFrames && numFrames > 0) {
		currFrameIdx -= numFrames;
	}

	if (currFrameIdx < 0) {
		currFrameIdx = numFrames - 1;
	}

	std::cout << currFrameIdx << std::endl;

	sprite.setTextureRect(calculateRect());
}

void Spritesheet::setPosition(sf::Vector2<float> pos)
{
	sprite.setPosition(pos);
}

void Spritesheet::move(sf::Vector2<float> offset)
{
	sprite.move(offset);
}

void Spritesheet::setScale(sf::Vector2<float> scale)
{
	sprite.setScale(scale);
}

void Spritesheet::scale(sf::Vector2<float> factor)
{
	sprite.scale(factor);
}

void Spritesheet::setSize(sf::Vector2<float> size)
{
	float scaleX = size.x / frameSize.x;
	float scaleY = size.y / frameSize.y;

	sprite.setScale({ scaleX, scaleY });
}

void Spritesheet::setRotation(sf::Angle angle)
{
	sprite.setRotation(angle);
}

void Spritesheet::rotate(float degrees)
{
	sprite.rotate(sf::degrees(degrees));
}

void Spritesheet::setOrigin(sf::Vector2<float> origin)
{
	sprite.setOrigin(origin);
}

void Spritesheet::setColor(sf::Color color)
{
	sprite.setColor(color);
}

void Spritesheet::setAnimation(Animation animation)
{
	if (this->checkAnimation(animation)) {
		currAnimation = animation;
		this->setFrame(animation.startFrame);
		isAnimated = true;
	}
}

bool Spritesheet::contains(sf::Vector2<float> pos)
{
	if (sprite.getGlobalBounds().contains(pos)) {
		return true;
	}

	return false;
}

sf::Vector2<float> Spritesheet::getPosition()
{
	return sprite.getPosition();
}

sf::Vector2<float> Spritesheet::getScale()
{
	return sprite.getScale();
}

sf::Angle Spritesheet::getRotation()
{
	return sprite.getRotation();
}

sf::Vector2<float> Spritesheet::getOrigin()
{
	return sprite.getOrigin();
}

sf::Rect<float> Spritesheet::getLocalBounds()
{
	return sprite.getLocalBounds();
}

sf::Rect<float> Spritesheet::getGlobalBounds()
{
	return sprite.getGlobalBounds();
}

sf::Texture* Spritesheet::getTexture()
{
	return texture;
}

sf::Rect<int> Spritesheet::getTextureRect()
{
	return sprite.getTextureRect();
}

void Spritesheet::setDimensions(sf::Vector2<unsigned int> newDimensions) {
	dimensions = newDimensions;

	frameSize.x = texture->getSize().x / dimensions.x;
	frameSize.y = texture->getSize().y / dimensions.y;
	sprite.setTextureRect({ {0,0},{(int)frameSize.x, (int)frameSize.y } });
	currFrameIdx = 0;
	numFrames = dimensions.x * dimensions.y;
}

void Spritesheet::setTexture(sf::Texture* newTexture) {
	if (!newTexture) {
		return;
	}

	texture = newTexture;
	sprite.setTexture(*texture);
	frameSize.x = texture->getSize().x / dimensions.x;
	frameSize.y = texture->getSize().y / dimensions.y;
	sprite.setTextureRect({ {0,0},{(int)frameSize.x, (int)frameSize.y } });
	currFrameIdx = 0;
}

void Spritesheet::setFrameSize(unsigned int sizeX, unsigned int sizeY)
{
	if (!texture) {
		std::cout << "Could not set spritesheet texture size: texture not set" << std::endl;
		return;
	}

	dimensions.x = texture->getSize().x / sizeX;
	dimensions.y = texture->getSize().y / sizeY;

	frameSize.x = texture->getSize().x / dimensions.x;
	frameSize.y = texture->getSize().y / dimensions.y;
	sprite.setTextureRect({ {0,0},{(int)frameSize.x, (int)frameSize.y} });
	currFrameIdx = 0;
	numFrames = dimensions.x * dimensions.y;
}

void Spritesheet::draw(sf::RenderWindow& win) {
	win.draw(sprite);

	if (areBoundsDrawn) { this->drawBounds(win); }
}


void Spritesheet::update(float dt) {
	//Check for valid animation
	if (isAnimated) {
		animTimer += dt;

		if (animTimer >= (1 / currAnimation.fps)) {
			currAnimation.adv();
			this->setFrame(currAnimation.currFrame);
			animTimer = 0.f;
		}

		if (currAnimation.isDone) {
			isAnimated = false;
		}
	}
}
