#include "Helicopter.h"

void Helicopter::init()
{
	
}

void Helicopter::drawDebug(sf::RenderWindow& win)
{
	sf::Color lineColor = sf::Color::Red;

	//Momentum Line
	sf::Vector2<float> posA = sprite.getPosition();
	sf::Vector2<float> posB = sprite.getPosition() + mom;
	sf::Vertex mLine[2] = { {posA, lineColor, {0,0}},{posB, lineColor, {0,0}} };

	//Velocity Line
	lineColor = sf::Color::Yellow;
	posB = sprite.getPosition() + vel;
	sf::Vertex vLine[2] = { {posA, lineColor, {0,0}},{posB, lineColor, {0,0}} };
	
	//Text
	momText.setString("Momentum: [" + std::to_string(mom.x) + ", " + std::to_string(mom.y) + "]");
	velText.setString("Velocity: {" + std::to_string(vel.x) + ", " + std::to_string(vel.y) + "}");
	angleText.setString("Rotation: " + std::to_string(sprite.getRotation().asDegrees()) + " deg");
	posText.setString("Position: (" + std::to_string(sprite.getPosition().x) + ", " + std::to_string(sprite.getPosition().y) + ")");

	std::string weaponStr = "";
	float rng = utl::dist(mPos.x, mPos.y, sprite.getPosition().x, sprite.getPosition().y);

	weaponText.setFillColor(debugTextFgCol);
	if (weaponSel == 0) {
		weaponStr = "Rockets: " + std::to_string(rocketClipCurr) + "/" + std::to_string(rocketClipSize);
		if (isRocketReloading || rng > rocketMaxRange) { weaponText.setFillColor(sf::Color::Red); }
		
	}
	else if (weaponSel == 1) {
		weaponStr = "Cannon: -/-";
		if (rng > cannonMaxRange) {
			weaponText.setFillColor(sf::Color::Red);
		}
	}

	
	
	weaponStr += "\nRange: " + std::to_string(rng);
	weaponText.setString(weaponStr);

	killsText.setString("Targets Destroyed: " + std::to_string(kills));

	
	textBgs[0].setPosition(momText.getPosition()); textBgs[0].setSize(momText.getLocalBounds().size + sf::Vector2<float>(5.f, 5.f));
	textBgs[1].setPosition(velText.getPosition()); textBgs[1].setSize(velText.getLocalBounds().size + sf::Vector2<float>(5.f, 5.f));
	textBgs[2].setPosition(angleText.getPosition()); textBgs[2].setSize(angleText.getLocalBounds().size + sf::Vector2<float>(5.f, 5.f));
	textBgs[3].setPosition(posText.getPosition()); textBgs[3].setSize(posText.getLocalBounds().size + sf::Vector2<float>(5.f, 5.f));
	textBgs[4].setPosition(weaponText.getPosition()); textBgs[4].setSize(weaponText.getLocalBounds().size + sf::Vector2<float>(5.f, 5.f));
	textBgs[5].setPosition(killsText.getPosition()); textBgs[5].setSize(killsText.getLocalBounds().size + sf::Vector2<float>(5.f, 5.f));

	//Draw
	win.draw(vLine,2,sf::PrimitiveType::Lines);
	win.draw(mLine, 2, sf::PrimitiveType::Lines);
	win.setView(win.getDefaultView());
	win.draw(textBgs[0]);
	win.draw(textBgs[1]);
	win.draw(textBgs[2]);
	win.draw(textBgs[3]);
	win.draw(textBgs[4]);
	win.draw(textBgs[5]);
	win.draw(momText);
	win.draw(velText);
	win.draw(angleText);
	win.draw(posText);
	win.draw(weaponText);
	win.draw(killsText);
	win.draw(reloadIndBottom);
	win.draw(reloadIndTop);
}

void Helicopter::lookAtPoint(sf::Vector2<float> point)
{
	sf::Vector2<float> a = sprite.getPosition();
	float theta = std::atan2f(point.y - a.y, point.x - a.x);
	sprite.setRotation(sf::radians(theta + utl::degToRad(90.f)));
}

void Helicopter::fireWeapon(sf::Vector2<float> targetPos)
{
	int spreadPercent = rocketSpreadPercent;
	if (weaponSel == 1) { spreadPercent = cannonSpreadPercent; }

	//Calculate angle between heli pos and mouse pos
	float theta = std::atan2f(targetPos.y - sprite.getPosition().y, targetPos.x - sprite.getPosition().x);

	theta += (utl::randRange(-1 * spreadPercent / 2, spreadPercent / 2) / 100.f) * theta;

	
	sf::Vector2<float> projectilePos = sprite.getPosition();
	float mult = 20.f;

	if (weaponSel == 0) {
		rocketSide = !rocketSide; //swap rocket side
		if (rocketSide) {
			projectilePos.x += mult * std::cosf(theta + utl::degToRad(45.f));
			projectilePos.y += mult * std::sinf(theta + utl::degToRad(45.f));
		}
		else {
			projectilePos.x -= mult * std::cosf(theta + utl::degToRad(45.f));
			projectilePos.y -= mult * std::sinf(theta + utl::degToRad(45.f));
		}
	}

	float maxRange = rocketMaxRange;
	if (weaponSel == 1) { maxRange = cannonMaxRange; }
	float pSpeed = rocketSpeed;
	if (weaponSel == 1) { pSpeed = cannonSpeed; }
	sf::Texture* texture = rocketTexture;
	if (weaponSel == 1) { texture = bulletTexture; }
	int damage = rocketDmg;
	if (weaponSel == 1) { damage = cannonDmg; }
	sf::Vector2<float> pSize = rocketSize;
	if (weaponSel == 1) { pSize = bulletSize; }

	Projectile* p = new Projectile(projectilePos, theta); //is deleted by projectile manager
	p->setTexture(texture);
	p->setRotation(sprite.getRotation());
	p->setSize(pSize);
	p->setRange(utl::dist(projectilePos.x, projectilePos.y, targetPos.x, targetPos.y));
	p->setMaxRange(maxRange);
	p->setSpeed(pSpeed);
	p->setTeam(0);
	p->setDamage(damage);
	projectileMgr.addProjectile(p);
}

void Helicopter::updateController(float dt, sf::RenderWindow& win)
{
	

	//Get mouse position
	sf::Vector2<int> rawPos = sf::Mouse::getPosition(win);
	win.setView(*map_view);
	sf::Vector2<float> mapViewMousePos = win.mapPixelToCoords(rawPos);

	//Turn helicopter to look at mouse
	lookAtPoint(mapViewMousePos);
	//float theta = std::atan2f(mapViewMousePos.y - sprite.getPosition().y, mapViewMousePos.x - sprite.getPosition().x);
	float trimDeg = 90.f;
	float theta = sprite.getRotation().asRadians() + utl::radToDeg(trimDeg);

	//Update momentum
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		mom.x += std::cosf(utl::degToRad(270.f)) * delta * dt;
		mom.y += std::sinf(utl::degToRad(270.f)) * delta * dt;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		mom.x += std::cosf(utl::degToRad(180.f)) * delta * dt;
		mom.y += std::sinf(utl::degToRad(180.f)) * delta * dt;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		mom.x += std::cosf(utl::degToRad(90.f)) * delta * dt;
		mom.y += std::sinf(utl::degToRad(90.f)) * delta * dt;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		mom.x += std::cosf(utl::degToRad(0.f)) * delta * dt;
		mom.y += std::sinf(utl::degToRad(0.f)) * delta * dt;
	}

	//Add drag
	
	mom *= drag;

	//Clip at max momentum
	/*if (mom.x > maxMom.x) {
		mom.x = maxMom.x;
	}

	if (mom.y > maxMom.y) {
		mom.y = maxMom.y;
	}*/

	//Clip at low momentum
	float thresh = 0.1f;
	if (std::fabsf(std::sqrtf(std::powf(mom.x, 2) + std::powf(mom.y, 2))) <= thresh) {
		mom.x = 0.f;
		mom.y = 0.f;
	}


	//Update Velocity v = p / m
	vel.x = mom.x / mass;
	vel.y = mom.y / mass;

	sprite.move(vel);
}

void Helicopter::updateBasic(float dt, sf::RenderWindow& win)
{
	//Get mouse position
	sf::Vector2<int> rawPos = sf::Mouse::getPosition(win);
	win.setView(*map_view);
	sf::Vector2<float> mapViewMousePos = win.mapPixelToCoords(rawPos);

	
	sf::Vector2<float> a = sprite.getPosition();
	sf::Vector2<float> b = mapViewMousePos;
	float theta = std::atan2f(b.y - a.y, b.x - a.x);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		float x = std::cosf(theta) * speed * dt;
		float y = std::sinf(theta) * speed * dt;
		//sprite.move({ x, y });
		lastOffset = { 0,-1 * speed * dt };
		sprite.move({ 0,-1 * speed * dt });
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		float x = std::cosf(theta + utl::degToRad(90.f)) * speed * dt;
		float y = std::sinf(theta + utl::degToRad(90.f)) * speed * dt;
		//sprite.move({ -1 * x, -1 * y });
		lastOffset = { -1 * speed * dt, 0 };
		sprite.move({ -1 * speed * dt, 0 });
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		float x = std::cosf(theta) * speed * dt;
		float y = std::sinf(theta) * speed * dt;
		//sprite.move({ -1*x, -1*y });
		lastOffset = { 0,speed * dt };
		sprite.move({ 0,speed * dt });
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		float x = std::cosf(theta - utl::degToRad(90.f)) * speed * dt;
		float y = std::sinf(theta - utl::degToRad(90.f)) * speed * dt;
		//sprite.move({ -1 * x, -1 * y });
		lastOffset = { speed * dt,0 };
		sprite.move({ speed * dt,0 });
	}
}

void Helicopter::updateFireControl(float dt, sf::Vector2<float> mapViewMousePos)
{
	//Update reload timer regardless of selection
	if (isRocketReloading) {
		canRocketFire = false;
		isRocketWatingToFire = false;
		//Then update reload timer
		rocketReloadTimer += dt;

		//Update debug indicator
		reloadIndCurrScale += reloadIndScaleSpeed * dt;
		reloadIndTop.setScale({ reloadIndCurrScale ,reloadIndCurrScale });

		if (rocketReloadTimer >= rocketReloadInterval) {
			rocketReloadTimer = 0.f;
			isRocketReloading = false;
			canRocketFire = true;
			rocketClipCurr = rocketClipSize;
		}
	}

	if (weaponSel == 0) {
		this->updateRocketFireControl(dt, mapViewMousePos);
	}
	else if (weaponSel == 1) {
		this->updateCannonFireControl(dt, mapViewMousePos);
	}
}

void Helicopter::updateRocketFireControl(float dt, sf::Vector2<float> mapViewMousePos)
{
	//##  Rockets
	if (isRocketReloading) {
		canRocketFire = false;
		isRocketWatingToFire = false;
		//Then update reload timer
		rocketReloadTimer += dt;

		//Update debug indicator
		reloadIndCurrScale += reloadIndScaleSpeed * dt;
		reloadIndTop.setScale({ reloadIndCurrScale ,reloadIndCurrScale });

		if (rocketReloadTimer >= rocketReloadInterval) {
			rocketReloadTimer = 0.f;
			isRocketReloading = false;
			canRocketFire = true;
			rocketClipCurr = rocketClipSize;
		}
	}

	if (isRocketWatingToFire) {
		canRocketFire = false;
		//Then update firing timer
		rocketFireTimer += dt;

		if (rocketFireTimer >= rocketFireInterval) {
			rocketFireTimer = 0.f;
			isRocketWatingToFire = false;
			canRocketFire = true;
		}

	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && canRocketFire) {
		this->fireWeapon(mapViewMousePos);
		isRocketWatingToFire = true;
		rocketClipCurr -= 1;

		// Reload logic
		if (rocketClipCurr <= 0) {
			isRocketReloading = true;
			reloadIndTop.setScale({ 0.1f,0.1f });
			reloadIndCurrScale = 0.1f;
		}
	}
}

void Helicopter::updateCannonFireControl(float dt, sf::Vector2<float> mapViewMousePos)
{
	//##  Cannon
	if (isCannonWatingToFire) {
		canCannonFire = false;
		//Then update firing timer
		cannonFireTimer += dt;

		if (cannonFireTimer >= cannonFireInterval) {
			cannonFireTimer = 0.f;
			isCannonWatingToFire = false;
			canCannonFire = true;
		}

	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && canCannonFire) {
		this->fireWeapon(mapViewMousePos);
		isCannonWatingToFire = true;
	}
}

Helicopter::Helicopter()
{
	sprite.setDrawBounds(true);
	
	//Set up debug text
	if (!font.openFromFile("resource/font/roboto_regular.ttf")) { std::cout << "Could not open heli font" << std::endl; }
	momText.setFont(font);
	velText.setFont(font);
	angleText.setFont(font);

	momText.setFillColor(debugTextFgCol);
	momText.setCharacterSize(debugTextSize);
	momText.setString("Momentum: [0,0]");
	momText.setPosition({0.f,0.f});

	float padding = 10.f;
	float vtPosY = momText.getPosition().y + momText.getLocalBounds().size.y + padding;
	velText.setFillColor(debugTextFgCol);
	velText.setCharacterSize(debugTextSize);
	velText.setString("Velocity: {0,0}");
	velText.setPosition({ momText.getPosition().x,vtPosY});

	float angPosY = velText .getPosition().y + velText.getLocalBounds().size.y + padding;
	angleText.setFillColor(debugTextFgCol);
	angleText.setCharacterSize(debugTextSize);
	angleText.setString("Rotation: 0.0 deg");
	angleText.setPosition({ momText.getPosition().x,angPosY });

	float posPosY = angleText.getPosition().y + angleText.getLocalBounds().size.y + padding;
	posText.setFillColor(debugTextFgCol);
	posText.setCharacterSize(debugTextSize);
	posText.setString("Position: (0, 0)");
	posText.setPosition({ momText.getPosition().x,posPosY });

	weaponText.setFillColor(debugTextFgCol);
	weaponText.setCharacterSize(debugTextSize);
	weaponText.setString("Rockets: 8/8\nRange: ---");
	weaponText.setPosition({ momText.getPosition().x, 670.f});

	killsText.setFillColor(debugTextFgCol);
	killsText.setCharacterSize(debugTextSize);
	killsText.setString("Targets Destroyed: 0");
	killsText.setPosition({1050.f, 690.f});

	textBgs[0].setFillColor(debugTextBgCol);
	textBgs[1].setFillColor(debugTextBgCol);
	textBgs[2].setFillColor(debugTextBgCol);
	textBgs[3].setFillColor(debugTextBgCol);
	textBgs[4].setFillColor(debugTextBgCol);
	textBgs[5].setFillColor(debugTextBgCol);

	textBgs[0].setPosition(momText.getPosition()); textBgs[0].setSize(momText.getLocalBounds().size + sf::Vector2<float>(2.f,2.f));
	textBgs[1].setPosition(velText.getPosition()); textBgs[1].setSize(velText.getLocalBounds().size + sf::Vector2<float>(2.f, 2.f));
	textBgs[2].setPosition(angleText.getPosition()); textBgs[2].setSize(angleText.getLocalBounds().size + sf::Vector2<float>(2.f, 2.f));
	textBgs[3].setPosition(posText.getPosition()); textBgs[3].setSize(posText.getLocalBounds().size + sf::Vector2<float>(2.f, 2.f));
	textBgs[4].setPosition(weaponText.getPosition()); textBgs[4].setSize(weaponText.getLocalBounds().size + sf::Vector2<float>(2.f, 2.f));
	textBgs[5].setPosition(killsText.getPosition()); textBgs[5].setSize(killsText.getLocalBounds().size + sf::Vector2<float>(2.f, 2.f));

	reloadIndBottom.setFillColor(sf::Color::Red);
	reloadIndBottom.setRadius(15.f);
	reloadIndBottom.setOrigin(reloadIndBottom.getLocalBounds().getCenter());
	reloadIndBottom.setPosition({1200.f, 20.f});
	

	reloadIndTop.setFillColor(sf::Color::Green);
	reloadIndTop.setRadius(15.f);
	reloadIndTop.setOrigin(reloadIndTop.getLocalBounds().getCenter());
	reloadIndTop.setPosition(reloadIndBottom.getPosition());

	//scale per frame = (total scale dist) / (desired seconds to scale) * dt;
	reloadIndScaleSpeed = (1.0f - 0.1f) / rocketReloadInterval;
}

Helicopter::~Helicopter()
{
}

void Helicopter::update(float dt, sf::RenderWindow &win) {
	//Get mouse position
	sf::Vector2<int> rawPos = sf::Mouse::getPosition(win);
	win.setView(*map_view);
	sf::Vector2<float> mapViewMousePos = win.mapPixelToCoords(rawPos);
	mPos = mapViewMousePos;

	

	this->updateController(dt, win);
	this->updateFireControl(dt, mapViewMousePos);
	// this->updateBasic(dt, win);

	sprite.update(dt);

	projectileMgr.update(dt);
}

void Helicopter::poll(sf::RenderWindow& win, std::optional<sf::Event> event) {
	//## Mouse press
	if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
		sf::Vector2<int> rawPos = sf::Mouse::getPosition(win);
		win.setView(*map_view);
		sf::Vector2<float> mapViewMousePos = win.mapPixelToCoords(rawPos);
		

		//Left Button
		if (mouseButton->button == sf::Mouse::Button::Left) {
			
			
		}

	}

	//## Mouse release
	if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonReleased>()) {
		//Get position
		sf::Vector2<int> rawPos = sf::Mouse::getPosition(win);
		win.setView(*map_view);
		sf::Vector2<float> mapViewMousePos = win.mapPixelToCoords(rawPos);

		

		//Left
		if (mouseButton->button == sf::Mouse::Button::Left) {

		}

		//Right
		if (mouseButton->button == sf::Mouse::Button::Right) {
			
		}
	}
	//## Scroll Wheel
	if (const auto* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>())
	{
		switch (mouseWheelScrolled->wheel)
		{
		case sf::Mouse::Wheel::Vertical:
			if (mouseWheelScrolled->delta > 0) {
				weaponSel++;
			}
			else {
				weaponSel--;
			}

			if (weaponSel < 0) {
				weaponSel = 1;
			}

			if (weaponSel > 1) {
				weaponSel = 0;
			}
			break;
		case sf::Mouse::Wheel::Horizontal:
			
			break;
		}
	}

	//## Key Release
	if (const auto* keyRel = event->getIf<sf::Event::KeyReleased>()) {
		if (keyRel->code == sf::Keyboard::Key::Num1) {
			weaponSel = 0;
		}

		if (keyRel->code == sf::Keyboard::Key::Num2) {
			weaponSel = 1;
		}
	}

}

void Helicopter::draw(sf::RenderWindow& win) {
	projectileMgr.draw(win);	
	sprite.draw(win);
	this->drawDebug(win);
}

void Helicopter::setTexture(sf::Texture* texture) {
	sprite.setTexture(texture);
	sprite.setFrameSize(size, size);
	sprite.setFrame(0);
	sprite.setSize({(float)size*2,(float)size*2});
	sprite.setOrigin(sprite.getLocalBounds().getCenter());
}