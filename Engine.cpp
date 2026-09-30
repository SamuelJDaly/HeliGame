#include "Engine.h"
#include "Item.h"

void Engine::initWindow()
{
	win = new sf::RenderWindow;
	win->create(sf::VideoMode({ 1280, 720 }), "Utility AI Practice");
}

void Engine::initTextures()
{
	texReg = new TextureRegistry();

	texReg->addTexture("default", "resource/tex/nope.png");
	texReg->addTexture("tileset_test", "resource/tex/tileset_test.png");
	texReg->addTexture("tileset_0", "resource/tex/tileset_1.png");
	texReg->addTexture("tileset_3", "resource/tex/tileset_3.png");
	texReg->addTexture("tileset_4", "resource/tex/citysheet.png");
	texReg->addTexture("player", "resource/tex/redcircle.png");
	texReg->addTexture("star", "resource/tex/star.png");
	texReg->addTexture("medkit", "resource/tex/medkit.png");
	texReg->addTexture("heli_green", "resource/tex/heli_green.png");
	texReg->addTexture("heli_black", "resource/tex/heli_black.png");
	texReg->addTexture("rocket", "resource/tex/rocket_1.png");
	texReg->addTexture("bullet", "resource/tex/bullet.png");
	texReg->addTexture("truck_0", "resource/tex/truck_0.png");
}

void Engine::initState()
{
	//currState = new State_Game(texReg, win);
	//currState = new State_Heli(texReg, this->win);
	//currState = new State_LevelEditor(texReg, this->win);
	//currState = new State_Editor(textureHandler, this->win);
	//currState = new State_Menu(textureHandler, this->win);
	currState = new EditorState(texReg, this->win);
}

Engine::Engine()
{
	std::srand(std::time(0));
	this->initWindow();
	this->initTextures();
	this->initState();
	if (!ImGui::SFML::Init(*win)) {
		std::cout << "Failed to initiate ImGui" << std::endl;
	}
}

Engine::~Engine()
{
	ImGui::SFML::Shutdown();
	delete currState;
	delete texReg;
	delete win;
}

void Engine::poll()
{
	//# Handle Polled Events
	while (std::optional event = win->pollEvent()) {
		ImGui::SFML::ProcessEvent(*win, *event);
		//Window closure
		if (event->is<sf::Event::Closed>()) {
			win->close();
			isRunning = false;
		}

		if (auto* mouseButton = event->getIf<sf::Event::MouseButtonReleased>()) {
			if (mouseButton->button == sf::Mouse::Button::Left) {
				auto pos = sf::Mouse::getPosition(*win);

				//std::cout << "(" << pos.x << ", " << pos.y << ")" << std::endl;
			}
		}

		if (auto* keyRel = event->getIf<sf::Event::KeyReleased>()) {
			if (keyRel->code == sf::Keyboard::Key::Escape) {
				isMouseGrabbed = !isMouseGrabbed;
				win->setMouseCursorGrabbed(isMouseGrabbed);
			}
		}

		currState->poll(*win, event);
	}
}

void Engine::update()
{
	//## Update delta time
	deltaTime = mainClock.getElapsedTime().asSeconds();
	mainClock.restart();


	//Update Current State
	ImGui::SFML::Update(*win, sf::seconds(deltaTime));
	currState->update(deltaTime);

	//State Transition Logic
	if (currState->getFinished()) {
		//Transition to next state
		switch (currState->getNextState()) {
		case En_Gamestate::MENU:
			//De allocate last state
			delete currState;
			//Allocate new state
			//currState = new State_Menu(textureHandler, this->win);

			break;
		case En_Gamestate::GAME:
			//De allocate last state
			delete currState;
			//Allocate new state
			currState = new State_Game(texReg, this->win);

			break;
		case En_Gamestate::EDITOR:
			//De allocate last state
			delete currState;
			//Allocate new state
			//currState = new State_Editor(textureHandler, this->win);

			break;
		case En_Gamestate::END:
			win->close();
			isRunning = false;
			break;
		default:
			std::cerr << "Invalid State Transition" << std::endl;
			win->close();
			isRunning = false;
			break;
		}
	}
}



void Engine::draw()
{
	if (!currState) {
		isRunning = false;
		return;
	}

	win->clear(currState->getClearColor());

	currState->draw(*win);

	win->setView(win->getDefaultView());
	ImGui::SFML::Render(*win);

	win->display();
}



bool Engine::getRunning()
{
	return isRunning;
}
