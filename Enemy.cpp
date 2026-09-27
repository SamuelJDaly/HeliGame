#include "Enemy.h"

void Enemy::drawDebug(sf::RenderWindow& win)
{
	//Draw Hitbox
	sf::RectangleShape box;
	box.setFillColor(sf::Color::Transparent);
	box.setOutlineColor(sf::Color::Green);
	box.setOutlineThickness(1.f);
	box.setPosition(this->getHitbox().position);
	box.setSize(this->getHitbox().size);
	win.draw(box);

	//Draw sprite rectangle
	sf::RectangleShape box2;
	box2.setFillColor(sf::Color::Transparent);
	box2.setOutlineColor(sf::Color::Blue);
	box2.setOutlineThickness(1.f);
	box2.setPosition(sprite.getGlobalBounds().position);
	box2.setSize(sprite.getGlobalBounds().size);
	//win.draw(box2);

	//Draw Target line
	sf::Vector2<float> posA = sprite.getPosition();
	sf::Vector2<float> posB = targetPos;
	sf::Vertex line[2] = { {posA, sf::Color::Red, {0,0}},{posB, sf::Color::Red, {0,0}} };

	win.draw(line, 2, sf::PrimitiveType::Lines);
}

void Enemy::updateBasic(float dt)
{
	//## Random Wander
	if (!isTargetReached) {
		//Move to target
		float theta = std::atan2f(targetPos.y - sprite.getPosition().y, targetPos.x - sprite.getPosition().x);

		float x = std::cosf(theta) * speed * dt;
		float y = std::sinf(theta) * speed * dt;

		sprite.move({x,y});

		//Check for target reached
		if (utl::dist(sprite.getPosition().x,sprite.getPosition().y,targetPos.x,targetPos.y) <= 0.1) {
			isTargetReached = true;
		}
	}
	else {

		wanderTimer += dt;

		if (wanderTimer >= wanderIntervalCurr) {
			//Select new target
			float x = utl::randRange((int)sprite.getPosition().x - wanderRange, (int)sprite.getPosition().x + wanderRange);
			float y = utl::randRange((int)sprite.getPosition().y - wanderRange, (int)sprite.getPosition().y + wanderRange);

			if (x < 0.f) { x = 0.f; }
			if (y < 0.f) { y = 0.f; }


			wanderTimer = 0.f;
			wanderIntervalCurr = wanderIntervalBase;
			wanderIntervalCurr += wanderIntervalBase * (utl::randRange(-10, 10) / 100.f);
			targetPos = { x,y };
			isTargetReached = false;

			//Flip to face target
			if (targetPos.x < sprite.getPosition().x && !isFlipped) {
				sprite.scale({ -1.f,1.f });
				isFlipped = true;
			}
			else if(isFlipped) {
				sprite.scale({ -1.f,1.f });
				isFlipped = false;
			}
		}
	}
}

void Enemy::die()
{
	if (isDying) {
		return;
	}

	Animation explosionAnim = { 0,8,0,30,false,false };
	sprite.setAnimation(explosionAnim);
	//sprite.scale({ 1.5f,1.5f });
	isDying = true;
	std::cout << "Enemy: " << id << " is dying now" << std::endl;
}

Enemy::Enemy(int id)
{
	this->id = id;
}

Enemy::~Enemy()
{
}

void Enemy::update(float dt) {
	sprite.update(dt);

	this->updateBasic(dt);

	if (currHp <= 0) {
		this->die();
	}

	//Check death state
	if (isDying && !sprite.getAnimated()) {
		//Then death animation has finished
		isDead = true;
	}
}

void Enemy::draw(sf::RenderWindow& win) {
	
	sprite.draw(win);

	if (isDebugDrawn)
	{
		this->drawDebug(win);
	}
}

void Enemy::setTexture(sf::Texture* texture, bool resetHibox) {
	sprite.setTexture(texture);
	sprite.setFrameSize(32,32);
	sprite.setOrigin(sprite.getLocalBounds().getCenter());

	if (resetHibox)
	{
		hitBox = { {0,0},{32.f,32.f} };
	}
}

void Enemy::setPosition(sf::Vector2<float> pos) {
	sprite.setPosition(pos);

	if (isTargetReached) {
		targetPos = pos;
	}
}

void Enemy::setScale(sf::Vector2<float> factor)
{
	sprite.setScale(factor); 
	if (isFlipped) { sprite.scale({ -1.f,1.f }); }
	scaleFactor = factor;
}

sf::FloatRect Enemy::getHitbox()
{
	sf::Vector2<float> sf = {std::fabs(scaleFactor.x), std::fabs(scaleFactor.y)};
	sf::Vector2<float> boxPos = { hitBox.position.x + sprite.getPosition().x, hitBox.position.y + sprite.getPosition().y};
	
	sf::Vector2<float> boxSize = {hitBox.size.x * sf.x, hitBox.size.y * sf.y};

	//Correct for centered origin
	boxPos.x -= boxSize.x / 2.f;
	boxPos.y -= boxSize.y / 2.f;

	return { boxPos,boxSize };
}
