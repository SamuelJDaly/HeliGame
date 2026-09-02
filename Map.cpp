#include "Map.h"

void Map::align() {
	for (size_t i = 0; i < dimensions.y; i++) {
		for (size_t j = 0; j < dimensions.x; j++) {
			float posX = tileSize * j + offset.x;
			float posY = tileSize * i + offset.y;
			tiles.at(i)->at(j)->sprite.setSize({ (float)tileSize, (float)tileSize });
			tiles.at(i)->at(j)->sprite.setPosition({ posX,posY });
		}
	}
}

void Map::generateGrid()
{
	grid.clear();

	//Horizontal lines
	for (auto i = 0; i <= dimensions.y; i++) {
		sf::Vector2<float> posA = { offset.x - (tileSize / 2), i * tileSize + offset.y - (tileSize / 2) };
		sf::Vector2<float> posB = { dimensions.x * tileSize + offset.x - (tileSize / 2), i * tileSize + offset.y - (tileSize / 2) };
		grid.push_back({ posA, gridColor,{0,0} });
		grid.push_back({ posB, gridColor, {0,0} });
	}

	//Vertical lines
	for (auto i = 0; i <= dimensions.x; i++) {
		sf::Vector2<float> posA = { i * tileSize + offset.x - (tileSize / 2), offset.y - (tileSize / 2) };
		sf::Vector2<float> posB = { i * tileSize + offset.x - (tileSize / 2), dimensions.y * tileSize + offset.y - (tileSize / 2) };
		grid.push_back({ posA, gridColor,{0,0} });
		grid.push_back({ posB, gridColor, {0,0} });
	}
}

Map::Map(TextureRegistry* textureRegistry)
{
	texReg = textureRegistry;

	//Create Fog Texture
	fogTexture = new sf::Texture();
	sf::Image img;
	img.resize({ 1, 1 }, sf::Color::Black);
	if (!fogTexture->loadFromImage(img)) {
		std::cout << "Could not load fog texture!" << std::endl;
	}
}

Map::~Map()
{
	for (unsigned int i = 0; i < dimensions.y; i++) {
		for (unsigned int j = 0; j < dimensions.x; j++) {
			delete tiles.at(i)->at(j);
		}

		delete tiles.at(i);
	}

	tiles.clear();

	delete fogTexture;
}


std::vector<sf::Vector2<float>> Map::pathfind(sf::Vector2<int> origin, sf::Vector2<int> target)
{
	std::vector<sf::Vector2<float>> path = {};
	if (origin == target) {
		return path;
	}



	//Check constraints
	if (origin.x < 0 || origin.x >= (int)dimensions.x || origin.y < 0 || origin.y >= (int)dimensions.y) {
		return path;
	}

	if (target.x < 0 || target.x >= (int)dimensions.x || target.y < 0 || target.y >= (int)dimensions.y) {
		return path;
	}

	//Check if origin or target is impassable
	if (!tiles.at(origin.y)->at(origin.x)->isPassable || !tiles.at(target.y)->at(target.x)->isPassable) {
		return path;
	}


	//Do A*
	std::vector<std::vector<bool>> passableMap;
	for (unsigned int i = 0; i < dimensions.y; i++) {
		passableMap.push_back(std::vector<bool>());
		for (unsigned int j = 0; j < dimensions.x; j++) {
			bool isPassable = tiles.at(i)->at(j)->isPassable;
			passableMap.at(i).push_back(isPassable);
		}
	}

	std::vector<std::pair<int, int>> pathIndicies = utl::aStar({ origin.x, origin.y }, { target.x, target.y }, passableMap);

	for (size_t i = 0; i < pathIndicies.size(); i++) {
		int x = pathIndicies.at(i).first * tileSize + offset.x;
		int y = pathIndicies.at(i).second * tileSize + offset.y;
		path.push_back({ (float)x,(float)y });
	}


	return path;
}

void Map::setOffset(sf::Vector2<float> newOffset) {
	sf::Vector2<float> diff = newOffset - offset;
	offset = newOffset;
	this->generateGrid();
	this->align();
	for (auto v : grid) {
		v.position.x += diff.x;
		v.position.y += diff.y;
	}
}

sf::Vector2<int> Map::posToTileIdx(sf::Vector2<float> pos)
{
	//Account for centered origin
	pos.x += (float)tileSize / 2.f;
	pos.y += (float)tileSize / 2.f;

	pos.x -= offset.x;
	pos.y -= offset.y;

	//Get index
	sf::Vector2<int> idx = { (int)std::floor(pos.x / tileSize),(int)std::floor(pos.y / tileSize) };

	//Should flip this so it fails early and checks less conditions on avg
	if (idx.x >= 0 && idx.y >= 0 && idx.x < (int)dimensions.x && idx.y < (int)dimensions.y) {
		return idx;
	}

	return { -1,-1 };
}

sf::Vector2<float> Map::tileIdxToPos(sf::Vector2<int> idx)
{
	//Check for valid idx
	if (idx.x >= dimensions.x || idx.y >= dimensions.y || idx.x < 0 || idx.y < 0) {
		std::cout << "Cannot get map position at index, index out of range..." << std::endl;
		return { -1.f,-1.f };
	}

	//If idx valid return position of tile at idx
	return tiles.at(idx.y)->at(idx.x)->sprite.getPosition();
}

sf::Vector2<float> Map::getSize()
{
	return { (float)dimensions.x * (float)tileSize, (float)dimensions.y * (float)tileSize };
}

int Map::loadFromFile(std::string filename)
{
	std::cout << "Loading map: " << filename << std::endl;

	std::ifstream inFile;
	inFile.open(filename);
	if (!inFile.is_open()) {
		std::cout << "Could not open map: " << filename << std::endl;
		return -1;
	}

	//Read filetype
	int fType;
	inFile >> fType;
	if (fType != 100) {
		std::cout << "Could not load map: " << filename << ", incorrect type..." << std::endl;
		return -1;
	}

	//Read texture info
	std::string textureName; int textureSize = 16;
	inFile >> textureName;
	inFile >> textureSize;
	inFile >> tileSize;

	//Read dimensions
	inFile >> dimensions.x >> dimensions.y;


	//Read Tiles
	Tile* t; int type; int pass; float cost; float rotation;

	for (unsigned int i = 0; i < dimensions.y; i++) {
		tiles.push_back(new std::vector<Tile*>());
		for (unsigned j = 0; j < dimensions.x; j++) {
			t = new Tile();
			t->sprite.setTexture(texReg->lookup(textureName));
			t->sprite.setFrameSize(32, 32);
			t->sprite.setSize({ (float)tileSize, (float)tileSize });
			t->sprite.setPosition({ (float)j * textureSize + offset.x, (float)i * textureSize + offset.y });
			t->sprite.setOrigin({ t->sprite.getLocalBounds().size.x / 2.f, t->sprite.getLocalBounds().size.y / 2.f });
			t->pos = { j,i };
			tiles.at(i)->push_back(t);

			inFile >> *t;
			

			/*char c;
			inFile >> c;
			inFile >> type >> pass >> cost >> rotation;
			t->sprite.setTexture(texReg->lookup(textureName));
			t->sprite.setFrameSize(32, 32);
			t->sprite.setSize({ (float)tileSize, (float)tileSize });
			t->sprite.setPosition({ (float)j * textureSize + offset.x, (float)i * textureSize + offset.y });
			t->sprite.setOrigin({ t->sprite.getLocalBounds().size.x / 2.f, t->sprite.getLocalBounds().size.y / 2.f });
			t->sprite.setRotation(sf::degrees(rotation));
			t->type = type;
			t->sprite.setFrame(type);
			t->isPassable = pass;
			t->moveCost = cost;
			t->pos = { j,i };
			tiles.at(i)->push_back(t);
			inFile >> c;*/
		}
	}

	//Close file
	inFile.close();

	this->generateGrid();

	std::cout << "Map Loaded..." << std::endl;

	return 1;
}

sf::Vector2<float> Map::getOffset()
{
	return offset;
}

Tile* Map::tileAtIdx(size_t x, size_t y)
{
	if (x >= dimensions.x || y >= dimensions.y) {
		return nullptr;
	}
	return tiles.at(y)->at(x);
}

Tile* Map::tileAtIdx(std::pair<int, int> idx)
{
	if ((unsigned int)idx.first >= dimensions.x || (unsigned int)idx.second >= dimensions.y) {
		return nullptr;
	}

	if (idx.second < 0 || idx.first < 0) {
		return nullptr;

	}
	return tiles.at(idx.second)->at(idx.first);
}

int Map::mnhtnDist(sf::Vector2<float> a, sf::Vector2<float> b)
{
	auto idxA = posToTileIdx(a);
	auto idxB = posToTileIdx(b);

	return std::fabs(idxA.x - idxB.x) + std::fabs(idxA.y - idxB.y);
}


std::vector<sf::Vector2<int>> Map::tileIdxInRange(int range, sf::Vector2<int> og, bool includeOG)
{
	std::vector < sf::Vector2<int>> idxs;
	int minX = og.x - range;
	int maxX = og.x + range;
	int minY = og.y - range;
	int maxY = og.y + range;

	//Clamp Range
	if (minX < 0) { minX = 0; }
	if (maxX >= dimensions.x) { maxX = dimensions.x - 1; }
	if (minY < 0) { minY = 0; }
	if (maxY >= dimensions.y) { maxY = dimensions.y - 1; }


	//Calculate tiles
	for (int i = minY; i <= maxY; i++) {
		for (int j = minX; j <= maxX; j++) {
			if (og.x == j && og.y == i) {
				if (includeOG) { idxs.push_back({ j,i }); }
			}
			else if ((int)std::floor(utl::dist((float)og.x, (float)og.y, (float)j, (float)i)) <= range) {
				idxs.push_back({ j,i });
			}
		}
	}

	return idxs;
}
void Map::Poll(sf::RenderWindow& win, std::optional<sf::Event> event)
{
	for (unsigned int i = 0; i < dimensions.y; i++) {
		for (unsigned int j = 0; j < dimensions.x; j++) {
			tiles.at(i)->at(j)->Poll(win, event);
		}
	}
}

void Map::Update()
{

}

void Map::Draw(sf::RenderWindow& win)
{
	//Tiles
	for (unsigned int i = 0; i < dimensions.y; i++) {
		for (unsigned int j = 0; j < dimensions.x; j++) {
			tiles.at(i)->at(j)->draw(win);
		}
	}

	//Grid
	if (drawGrid) {
		for (auto i = 0; i < grid.size(); i += 2) {
			sf::Vertex line[2] = { grid.at(i),grid.at(i + 1) };
			win.draw(line, 2, sf::PrimitiveType::Lines);
		}
	}
}
