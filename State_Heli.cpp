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

	map = new Map(texReg);
	map->loadFromFile("resource/map_multi.txt");
	map->setGridDraw(false);

	//## Player
	player = new Helicopter();
	player->setTexture(texReg->lookup("heli_green"));
	//player->setPos({ win->getSize().x / 2.f , win->getSize().y / 2.f });
	player->setPos(view_map.getCenter());
	player->setMapView(&view_map);
	player->setRocketTexture(texReg->lookup("rocket"));
	player->setBulletTexture(texReg->lookup("bullet"));

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
	for (auto p : player->getProjectileMgr()->getProjectiles()) {
		if (p->getTeam() != 0 || p->getDying()) {
			//only consider player projectiles that are not already exploding
			continue;
		}
		for (auto e : enemies) {
			std::optional<sf::FloatRect> intersection = e->getHitbox().findIntersection(p->getGlobalBounds());
			if (intersection.has_value()) {
				p->detonate();
				e->modCurrHP(-1*p->getDamage());

				std::cout << "Enemy: " << e->getID() << " just took " << p->getDamage() << " damage: " << e->getCurrHp() << std::endl;
			}
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
}


void State_Heli::update(float dt) {
	player->update(dt, *win);
	this->updateEnemies(dt);
	this->updateCollision(dt);
	this->updateTest(dt);
	this->updateCamera(dt);
}


void State_Heli::poll(sf::RenderWindow& win, std::optional<sf::Event> event) {
	player->poll(win, event);
}

void State_Heli::draw(sf::RenderWindow& win) {
	win.setView(win.getDefaultView());


	win.setView(view_map);
	map->Draw(win);
	for (auto e : enemies) {
		e->draw(win);
	}
	player->draw(win);
}