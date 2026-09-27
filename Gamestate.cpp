#include "Gamestate.h"


//################################################################################################################
//				COMMON FUNCTIONS
////##############################################################################################################
bool Gamestate::getFinished()
{
	return isFinished;
}

En_Gamestate Gamestate::getNextState()
{
	return nextState;
}

//################################################################################################################
//				GAME STATE FUNCTIONS
////##############################################################################################################


void State_Game::init()
{
	view_map = win->getDefaultView();

	map = new Map(texReg);
	map->setVeiw(&view_map);
	map->loadFromFile("resource/map_maze.txt");
	map->setGridColor({ 255,0,0,100 });
	map->setOffset({ 50.f,50.f });

	player = new Player(*texReg);
	player->setPos(map->tileIdxToPos({1,0}));
	player->setSize({25.f,25.f});

	if (!font.openFromFile("resource/font/roboto_regular.ttf")) {
		std::cout << "Could not load font" << std::endl;
	}

	scoreText.setFont(font);
	scoreText.setCharacterSize(20);
	scoreText.setString("Score: 0");
	scoreText.setPosition({0,0});

	hpText.setFont(font);
	hpText.setCharacterSize(20);
	hpText.setString("HP: 100/100");
	hpText.setPosition({ 0,scoreText.getGlobalBounds().size.y + 10});

	//Place Stars
	Item* star = new Item();
	star->setTexture(texReg->lookup("star"));
	star->setSize({16.f,16.f});
	star->setPosition(map->tileIdxToPos({6,1}));
	star->setEffect({50,0});

	Item* medpack = new Item();
	medpack->setTexture(texReg->lookup("medkit"));
	medpack->setSize({ 16.f,16.f });
	medpack->setPosition(map->tileIdxToPos({ 2,3 }));
	medpack->setEffect({ 0,25 });

	items.push_back(star);
	items.push_back(medpack);

}


void State_Game::moveCamera(sf::Vector2<float> offset)
{
	cameraOffset += offset;
	view_map.move(offset);
}

void State_Game::updateCamera(float dt)
{
	//Pan
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		this->moveCamera({ 0, -1.f * panSpeed * currZoom * dt });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		this->moveCamera({ 0, panSpeed * currZoom * dt });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		this->moveCamera({ -1.f * panSpeed * currZoom * dt, 0 });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		this->moveCamera({ panSpeed * currZoom * dt, 0 });
	}
}

void State_Game::updateScore()
{
	scoreText.setString("Score: " + std::to_string(score));
	int posX = (int)(scoreText.getGlobalBounds().size.x / 2.f - (.5 * scoreText.getGlobalBounds().size.x / 2.f));
	posX = 0.f;
	int posY = (int)(scoreText.getGlobalBounds().size.y / 2.f - (.5 * scoreText.getGlobalBounds().size.y / 2.f));
	scoreText.setPosition({(float)posX,(float)posY});
}

void State_Game::updateHPText()
{
	hpText.setString("HP: " + std::to_string(player->getHpLeft()) + "/" + std::to_string(player->getBaseHp()));
}

void State_Game::updateItemCollision()
{
	float threshold = 0.1; //How close item and player have to be to be considered touching
	for (auto it = items.begin(); it != items.end();) {
		auto posA = player->getPos();
		auto posB = (*it)->getPosition();

		if (utl::dist(posA.x, posA.y, posB.x, posB.y) <= threshold) {
			//Then player is touching this item, trigger effects and remove item from list
			score += (*it)->getEffect().score;
			player->modHpLeft((*it)->getEffect().hpRestore);

			//Remove item and update iterator
			delete (*it);
			it = items.erase(it);
		}
		else {
			//Iterate
			++it;
		}
	}
}

void State_Game::updatePlayerInput()
{
	if (!player->getMoving()) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
			sf::Vector2<int> og = map->posToTileIdx(player->getPos());
			sf::Vector2<int> target = {og.x, og.y-1};
			std::vector<sf::Vector2<float>> path = map->pathfind(og, target);

			if (path.size() >= 1) { player->pathTo(path); }

		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
			sf::Vector2<int> og = map->posToTileIdx(player->getPos());
			sf::Vector2<int> target = { og.x - 1, og.y };
			std::vector<sf::Vector2<float>> path = map->pathfind(og, target);

			if (path.size() >= 1) { player->pathTo(path); }
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
			sf::Vector2<int> og = map->posToTileIdx(player->getPos());
			sf::Vector2<int> target = { og.x, og.y+1 };
			std::vector<sf::Vector2<float>> path = map->pathfind(og, target);

			if (path.size() >= 1) { player->pathTo(path); }
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
			sf::Vector2<int> og = map->posToTileIdx(player->getPos());
			sf::Vector2<int> target = { og.x + 1, og.y };
			std::vector<sf::Vector2<float>> path = map->pathfind(og, target);

			if (path.size() >= 1) { player->pathTo(path); }
		}
	}
}

void State_Game::pollPlayerInput(sf::RenderWindow& win, std::optional<sf::Event> event)
{
	if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonReleased>()) {
		//Get position
		sf::Vector2<int> rawPos = sf::Mouse::getPosition(win);
		win.setView(view_map);
		sf::Vector2<float> mapViewMousePos = win.mapPixelToCoords(rawPos);

		//Left
		if (mouseButton->button == sf::Mouse::Button::Left) {


		}


		//Right
		if (mouseButton->button == sf::Mouse::Button::Right) {
			//Player Movement
			//Check for valid position
			sf::Vector2<int> og = map->posToTileIdx(player->getPos());
			sf::Vector2<int> target = map->posToTileIdx(mapViewMousePos);
			std::vector<sf::Vector2<float>> path = map->pathfind(og, target);
			std::cout << "[" << target.x << ", " << target.y << "]" << std::endl;


			if (path.size() >= 1) {
				player->pathTo(path);
			}
		}
	}
}

//################################### CONSTRUCTOR AND DESTRUCTOR

State_Game::State_Game(TextureRegistry* textureRegistry, sf::RenderWindow* window)
	: Gamestate(textureRegistry, window)
{
	texReg = textureRegistry;
	win = window;
	this->init();
}

State_Game::~State_Game()
{
	delete map;
	delete player;

	for (auto i : items) {
		delete i;
	}

	items.clear();
}


//##################################### GENERAL

void State_Game::endTurn()
{
	player->endTurn();
}

void State_Game::beginTurn()
{
	player->beginTurn();
}

//##################################### UPDATE
void State_Game::update(float dt) {
	this->updateCamera(dt);
	map->Update();
	player->update(dt);
	this->updateItemCollision();
	this->updateScore();
	this->updateHPText();
	this->updatePlayerInput();
}

//##################################### POLL
void State_Game::poll(sf::RenderWindow& win, std::optional<sf::Event> event)
{
	map->Poll(win, event);

	if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonReleased>()) {
		//Get position
		sf::Vector2<int> rawPos = sf::Mouse::getPosition(win);
		win.setView(view_map);
		sf::Vector2<float> mapViewMousePos = win.mapPixelToCoords(rawPos);

		//Left
		if (mouseButton->button == sf::Mouse::Button::Left) {


		}

		//Right
		if (mouseButton->button == sf::Mouse::Button::Right) {
			
		}
	}
	
	if (const auto* keyRel = event->getIf<sf::Event::KeyReleased>()) {
		if (keyRel->code == sf::Keyboard::Key::Enter) {
			this->endTurn();
			this->beginTurn();
		}

		if (keyRel->code == sf::Keyboard::Key::Add) { score += 10; }
		if (keyRel->code == sf::Keyboard::Key::Subtract) { score -= 10; }
		if (keyRel->code == sf::Keyboard::Key::RBracket) { player->modHpLeft(10); }
		if (keyRel->code == sf::Keyboard::Key::LBracket) { player->modHpLeft(-10); }
	}

	this->pollPlayerInput(win,event);
}

//##################################### DRAW
void State_Game::draw(sf::RenderWindow& win) {
	
	win.setView(view_map);
	map->Draw(win);
	for (auto i : items) {
		i->draw(win);
	}
	player->draw(win);

	win.setView(win.getDefaultView());
	win.draw(scoreText);
	win.draw(hpText);
}
