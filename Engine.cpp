#include "Engine.h"
#include "Item.h"
#include "AudioManager.h"

void Engine::initWindow()
{
	win = new sf::RenderWindow;
	win->create(sf::VideoMode({ 1280, 720 }), "Helicopter Game");
}

void Engine::initTextures()
{
	texReg.loadFromFile(textureManifestPath);

	texReg.addTexture("default", "resource/tex/nope.png");
	texReg.addTexture("tileset_test", "resource/tex/tileset_test.png");
	texReg.addTexture("tileset_0", "resource/tex/tileset_1.png");
	texReg.addTexture("tileset_3", "resource/tex/tileset_3.png");
	texReg.addTexture("tileset_4", "resource/tex/citysheet.png");
	texReg.addTexture("player", "resource/tex/redcircle.png");
	texReg.addTexture("star", "resource/tex/star.png");
	texReg.addTexture("medkit", "resource/tex/medkit.png");
	texReg.addTexture("heli_green", "resource/tex/heli_green.png");
	texReg.addTexture("heli_black", "resource/tex/heli_black.png");
	texReg.addTexture("rocket", "resource/tex/rocket_1.png");
	texReg.addTexture("rocket2", "resource/tex/rocket_2.png");
	texReg.addTexture("bullet", "resource/tex/bullet.png");
	texReg.addTexture("truck_0", "resource/tex/truck_0.png");
	texReg.addTexture("cursor_sel", "resource/tex/cursor_sel.png");
	texReg.addTexture("cursor_brush", "resource/tex/cursor_brush.png");
	texReg.addTexture("circleIcon", "resource/tex/circleIcon.png");
	texReg.addTexture("squareIcon", "resource/tex/squareIcon.png");
	texReg.addTexture("explosion_0", "resource/tex/explosion_0.png");
}

void Engine::initAudio()
{
	audioManager.registerSound("rocket_fire", "resource/sfx/rocket_0.wav");
}

void Engine::initState()
{
	//currState = new State_Game(texReg, win);
	currState = new State_Heli(&texReg,&audioManager, this->win);
	//currState = new State_Menu(textureHandler, this->win);
	//currState = new EditorState(texReg, this->win);
}

Engine::Engine()
{
	std::srand(std::time(0));
	this->initWindow();
	this->initTextures();
	this->initAudio();
	this->initState();
	if (!ImGui::SFML::Init(*win)) {
		std::cout << "Failed to initiate ImGui" << std::endl;
	}

	if (!fnt.openFromFile("resource/font/jmhtype.ttf"))
	{
		std::cout << "Could not load debug font" << std::endl;
	}

	fpsText.setFont(fnt);
	fpsText.setCharacterSize(14);
	fpsText.setString("FPS: 0");
	fpsText.setOrigin({ fpsText.getLocalBounds().size.x / 2 ,fpsText.getLocalBounds().size.y / 2 });
	fpsText.setPosition({ (float)(win->getSize().x / 2),50.f});
}

Engine::~Engine()
{
	ImGui::SFML::Shutdown();
	delete currState;
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

	//## Update fps info
	//I suspect this is not 100% accurate, but its good enough for my purposes
	frameTimer += deltaTime;
	frameCount++;
	if (frameTimer >= 1.0)
	{
		frameTimer = 0.f;
		fps = frameCount;
		frameCount = 0;
	}

	fpsText.setString("FPS: " + std::to_string(fps));

	//Update Current State
	ImGui::SFML::Update(*win, sf::seconds(deltaTime));
	currState->update(deltaTime);
	audioManager.update();

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
			currState = new State_Heli(&texReg,&audioManager, this->win);

			break;
		case En_Gamestate::EDITOR:
			//De allocate last state
			delete currState;
			//Allocate new state
			currState = new EditorState(&texReg, &audioManager,  this->win);

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

	if (doShowDebugInfo)
	{
		win->draw(fpsText);
	}

	win->display();
}

void Engine::writeTextures()
{
	texReg.writeToFile(textureManifestPath);
}



bool Engine::getRunning()
{
	return isRunning;
}
