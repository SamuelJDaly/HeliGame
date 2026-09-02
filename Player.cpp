#include "Player.h"

void Player::init(TextureRegistry& texReg)
{
	sprite.setTexture(texReg.lookup("player"));
	sprite.setFrameSize(32, 32);
	sprite.setOrigin({ 16,16 });
	
}

void Player::drawPath(sf::RenderWindow& win)
{

	//for (int i = path.size()-1; i >= 0; i--) {
	//	
	//	//Determine node color
	//	

	//	if (i == path.size()-1) {
	//		//Draw line from player
	//		sf::Vertex a = { sprite.getPosition(), {0,255,0,255}, {0,0} };
	//		sf::Vertex b = { path.at(i), {0,255,0,255}, {0,0}};

	//		sf::Vertex line[] = {a,b};
	//		win.draw(line, 2, sf::PrimitiveType::Lines);

	//		sf::CircleShape circle;
	//		circle.setPosition(path.at(i));
	//		circle.setRadius(5);
	//		circle.setOrigin(circle.getGeometricCenter());
	//		circle.setFillColor(sf::Color::Green);
	//		win.draw(circle);
	//	}
	//	else {
	//		//Draw path point
	//		if (i == 0) {
	//			//Draw last node as square
	//			sf::RectangleShape rect;
	//			rect.setSize({ 10,10 });
	//			rect.setFillColor(sf::Color::Green);
	//			rect.setOrigin(rect.getGeometricCenter());
	//			rect.setPosition(path.at(i));
	//			win.draw(rect);
	//		}
	//		else {
	//			sf::CircleShape circle;
	//			circle.setPosition(path.at(i));
	//			circle.setRadius(5);
	//			circle.setOrigin(circle.getGeometricCenter());
	//			circle.setFillColor(sf::Color::Green);
	//			win.draw(circle);
	//		}


	//		//Draw line from last point
	//		sf::Vertex a = { path.at(i + 1), {0,255,0,255}, {0,0} };
	//		sf::Vertex b = { path.at(i), {0,255,0,255}, {0,0} };

	//		sf::Vertex line[] = { a,b };
	//		win.draw(line, 2, sf::PrimitiveType::Lines);
	//	}
	//	
	//	
	//}

	int idx = 1;
	for (auto it = path.begin(); it != path.end(); it++) {
		sf::Color nodeColor = sf::Color::Red;
		sf::Color nextNodeColor = sf::Color::Red;

		path.size()-idx < baseMp ? nodeColor = sf::Color::Yellow : nodeColor = sf::Color::Red;
		if (path.size() - idx < mpLeft) { nodeColor = sf::Color::Green; }
		path.size() - (idx+1) < baseMp ? nextNodeColor = sf::Color::Yellow : nextNodeColor = sf::Color::Red;
		if (path.size() - (idx+1) < mpLeft) { nextNodeColor = sf::Color::Green; }
		
		


		//Draw a line to next node
		if (std::next(it) == path.end()) {
			//Then this is the last node, draw a line to the player
			sf::Vertex a = { sprite.getPosition(), nextNodeColor , {0,0} };
			sf::Vertex b = { (*it), nodeColor, {0,0}};

			sf::Vertex line[] = {a,b};
			win.draw(line, 2, sf::PrimitiveType::Lines);
		}
		else {
			//Then this is NOT the last node, draw a line to it
			sf::Vertex a = { (*std::next(it)), nextNodeColor , {0,0}};
			sf::Vertex b = { (*it), nodeColor, {0,0} };

			sf::Vertex line[] = { a,b };
			win.draw(line, 2, sf::PrimitiveType::Lines);
		}


		if (it==path.begin()) {
			//Then this is the last node in the path (visually)
			//Draw a rectangle here
			sf::RectangleShape rect;
			rect.setSize({ 10,10 });
			rect.setFillColor(nodeColor);
			rect.setOrigin(rect.getGeometricCenter());
			rect.setPosition((*it));
			win.draw(rect);

		}
		else {
			//This is one of the nodes before the last node, draw a circle
			sf::CircleShape circle;
			circle.setPosition((*it));
			circle.setRadius(5);
			circle.setOrigin(circle.getGeometricCenter());
			circle.setFillColor(nodeColor);
			win.draw(circle);
		}

		idx++;
	}
}

Player::Player(TextureRegistry& texReg)
{
	this->init(texReg);
}

Player::~Player()
{
}

void Player::update(float dt) {
	//A bit of movement logic
	if (mpLeft <= 0) {
		doMovement = false;
	}

	//Check if path goal reached
	if (doMovement && !path.empty()) {
		sf::Vector2<float> a = sprite.getPosition();
		sf::Vector2<float> b = path.back();
		float theta = std::atan2f(b.y - a.y, b.x - a.x);
		int deg = (int)radToDeg(theta);
		deg = (deg + 360) % 360;

		//Check if subgoal reached
		if (dist(a, b) <= 0.1) {
			mpLeft -= 1;
			isMoving = false;
			path.pop_back();
		}
		else {
			//# Continue to subgoal
			//Movement
			float x = std::cosf(theta) * pathingSpeed * dt;
			float y = std::sinf(theta) * pathingSpeed * dt;
			sprite.move({ x, y });
			isMoving = true;

			//Direction
			sprite.setRotation(sf::degrees(deg + 90.f));
		}
	}
}

void Player::draw(sf::RenderWindow &window) {
	this->drawPath(window);
	sprite.draw(window);
}

void Player::endTurn() {
	doMovement = true;
}

void Player::beginTurn() {
	mpLeft = baseMp;
}

void Player::pathTo(std::vector<sf::Vector2<float>> newPath)
{
	path = newPath;
	doMovement = true;
}