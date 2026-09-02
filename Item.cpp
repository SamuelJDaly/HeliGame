#include "Item.h"

Item::Item()
{
}

Item::~Item()
{
}

void Item::draw(sf::RenderWindow& window) {
	sprite.draw(window);
}

void Item::setSize(sf::Vector2<float> size) {
	sprite.setSize(size);
}

void Item::setTexture(sf::Texture* newTexture)
{
	sprite.setTexture(newTexture);
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
}