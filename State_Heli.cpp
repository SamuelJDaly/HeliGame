#include "State_Heli.h"



void State_Heli::moveCamera(sf::Vector2<float> offset) {
	cameraOffset += offset;
	view_map.move(offset);
}

void State_Heli::updateCamera(float dt) {
	sf::Vector2<float> offset;


	//Get player offset from last frame
	
	offset = player->getPos() - player->getLastPos();
	view_map.setCenter(player->getPos());
	//moveCamera(player->getLastOffset());
}

void State_Heli::updateEnemies(float dt)
{
	/*for (auto it = enemies.begin(); it != enemies.end();)
	{

		if ((*it)->getDead()) {
			delete (*it);
			it = enemies.erase(it);
			player->addKill();
		}
		else {
			(*it)->update(dt);
			++it;
		}
	}*/

	auto c = [&](Enemy* e)
		{
			if (e->getDead())
			{
				player->addKill();
				return true;
			}

			return false;
		};

	enemies.erase(std::remove_if(enemies.begin(), enemies.end(), c), enemies.end());

	for (auto e : enemies)
	{
		e->update(dt);
	}
}

void State_Heli::init() {
	this->clearColor = sf::Color::Blue;
	view_map.setViewport({ {0.f,0.f},{1.f,1.f} });
	view_map.setSize({ (float)win->getSize().x, (float)win->getSize().y });



	map = new tmap::Tilemap(texReg);
	map->loadFromFile("resource/map_proto.txt");

	//## Projectile Manager
	projectileMgr.setView(view_map);

	//## Player
	player = new Helicopter();
	player->setTexture(texReg->lookup("heli_green"));
	//player->setPos({ win->getSize().x / 2.f , win->getSize().y / 2.f });
	player->setPos(view_map.getCenter());
	player->setMapView(&view_map);
	player->setRocketTexture(texReg->lookup("rocket2"));
	player->setBulletTexture(texReg->lookup("bullet"));
	player->setProjectileMgr(&projectileMgr);

	Animation heliAnim{0,7,0,60,false,true,-1,1};
	player->setAnimation(heliAnim);

}

void State_Heli::updateTest(float dt)
{
	//If number of enemies falls below 1 spawn some new ones
	if (enemies.empty()) {
		this->spawnRandomEnemies(5);
	}
}

void State_Heli::spawnRandomEnemies(int amt)
{
	for (int i = 0; i < amt; i++) {
		sf::Vector2<float> pos;
		pos.x = utl::randRange(20,(int)map->getSize().x - 20);
		pos.y = utl::randRange(20, (int)map->getSize().y - 20);

		Enemy* e = new Enemy(enemyCount);
		e->setTexture(texReg->lookup("truck_0"));
		e->setPosition(pos);
		e->setScale({1.5f,1.5f});
		e->setHitbox({ {0.f,0.f},{32.f,16.f} });
		e->SetIsDebugDrawn(true);
		enemyCount++;

		enemies.push_back(e);
	}

}

void State_Heli::updateCollision(float dt)
{
	//Naive method for now
	for (auto p : projectileMgr.getProjectiles()) {
		if (p->getTeam() != 0 || p->getDying()) {
			//only consider player projectiles that are not already exploding
			continue;
		}
		for (auto e : enemies) {
			std::optional<sf::FloatRect> intersection = e->getHitbox().findIntersection(p->getGlobalBounds());
			if (intersection.has_value()) {
				p->detonate();
				//e->modCurrHP(-1*p->getDamage());
				std::cout << "DIRECT HIT" << std::endl;
			}
		}
	}
}

void State_Heli::updateProjectiles(float dt)
{
	projectileMgr.update(dt);
	for (auto p : projectileMgr.getProjectiles())
	{
		if (p->getDoesExplode() && p->getWantsToExplode() && !p->getHasExploded())
		{
			//Then make an explosion
			Explosion* exp = new Explosion();
			exp->setTexture(texReg->lookup("explosion_0"));
			exp->setRadius(p->getAoeRadius());
			exp->setSize({ p->getAoeRadius() ,p->getAoeRadius()});
			exp->setPos(p->getPos());
			exp->setDmg(p->getDamage());
			exp->setAnimation({ 0,5,0,30,false,false });
			exp->playExplosion();
			p->setHasExploded(true);
			explosions.push_back(exp);

			//## Check for collisions (this sucks, but naive method again for now)
			sf::CircleShape c;
			c.setRadius(p->getAoeRadius());
			c.setPosition(p->getPos());

			for (auto e : enemies)
			{
				if (coll::circleRectIntersect(c,e->getGlobalBounds()))
				{
					//Then they collide
					std::cout << "EXPLOSION DAMAGE" << std::endl;
					float dist = utl::dist(p->getPos().x, p->getPos().y, e->getPos().x, e->getPos().y);
					//(max - actual) / max * baseDamage
					//float dmg = ((p->getAoeRadius() - dist) / p->getAoeRadius()) * p->getDamage();
					float dmg = p->getDamage();

					e->modCurrHP(-1 * dmg);
					std::cout << "Enemy: " << e->getID() << " just took " << dmg << " damage: " << e->getCurrHp() << std::endl;
				}
			}
		}
	}
}

void State_Heli::updateExplosions(float dt)
{
	for (auto it = explosions.begin(); it != explosions.end();)
	{

		if ((*it)->getDone())
		{
			delete (*it);
			it = explosions.erase(it);
		}
		else
		{
			(*it)->update(dt);
			++it;
		}
	}
}


State_Heli::State_Heli(TextureRegistry* textureRegistry, sf::RenderWindow* window)
	: Gamestate(textureRegistry, window)
{ 
	this->init();
}

State_Heli::~State_Heli() {
	delete map;
	delete player;

	for (auto e : explosions)
	{
		delete e;
	}
	explosions.clear();
}


void State_Heli::update(float dt) {
	player->update(dt, *win);
	this->updateEnemies(dt);
	this->updateCollision(dt);
	this->updateProjectiles(dt);
	this->updateExplosions(dt);
	this->updateTest(dt);
	this->updateCamera(dt);
}


void State_Heli::poll(sf::RenderWindow& win, std::optional<sf::Event> event) {
	player->poll(win, event);
}

void State_Heli::draw(sf::RenderWindow& win) {
	win.setView(win.getDefaultView());

	win.setView(view_map);
	map->draw(win);
	for (auto e : enemies) {
		e->draw(win);
	}

	projectileMgr.draw(win);
	player->draw(win);

	win.setView(view_map);
	for (auto e : explosions)
	{
		e->draw(win);
	}
}