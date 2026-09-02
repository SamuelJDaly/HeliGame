#include "Enemy.h"

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
			}
			else if(isFlipped) {
				sprite.scale({ -1.f,1.f });
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
}

void Enemy::setTexture(sf::Texture* texture) {
	sprite.setTexture(texture);
	sprite.setFrameSize(32,32);
}

void Enemy::setPosition(sf::Vector2<float> pos) {
	sprite.setPosition(pos);

	if (isTargetReached) {
		targetPos = pos;
	}
}