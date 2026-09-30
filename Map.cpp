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

Map::Map(TextureRegistry* textureRegistry, int tileSize, sf::Vector2<unsigned int> dims, std::string textureName, int textureSize)
{
	texReg = textureRegistry;
	this->tileSize = tileSize;
	this->dimensions = dims;

	tilesets.push_back({textureName, tileSize});

	for (unsigned int i = 0; i < dims.y; i++)
	{
		tiles.push_back(new std::vector<Tile*>());
		for (unsigned int j = 0; j < dims.x; j++)
		{
			Tile* t = new Tile();

			t->sheet = 0;
			t->type = 0;
			t->isPassable = 1;

			t->sprite.setTexture(texReg->lookup(textureName));
			t->sprite.setFrameSize(textureSize,textureSize);
			t->sprite.setFrame(0);


			t->sprite.setSize({ (float)tileSize, (float)tileSize });
			t->sprite.setPosition({ (float)j * tileSize + offset.x, (float)i * tileSize + offset.y });
			t->sprite.setOrigin({ t->sprite.getLocalBounds().size.x / 2.f, t->sprite.getLocalBounds().size.y / 2.f });
			t->pos = { j,i };

			tiles.at(i)->push_back(t);
		}
	}
}

Map::Map(const Map &deepCopy)
{
	this->dimensions = deepCopy.dimensions;
	this->texReg = deepCopy.texReg;
	this->tilesets = deepCopy.tilesets;
	this->numTilesets = deepCopy.numTilesets;
	//this->fogTexture = ... does not really matter right now, should implement later though for completeness
	this->drawGrid = deepCopy.drawGrid;
	this->gridColor = deepCopy.gridColor;
	this->offset = deepCopy.offset;
	this->tileSize = deepCopy.tileSize;
	this->view = deepCopy.view;
	
	//Copy tiles
	for (int i = 0; i < dimensions.y; i++)
	{
		tiles.push_back(new std::vector<Tile*>());
		for (int j = 0; j < dimensions.x; j++)
		{
			tiles.at(i)->push_back(new Tile(*deepCopy.tiles.at(i)->at(j)));
		}
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

	if (fogTexture)
	{
		delete fogTexture;
	}
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

	//Return -1 for axis if off map to top or left, return -2 if off map to bottom or right (this is insane, but itll work, just trust me bro)
	if (idx.x >= dimensions.x){ idx.x = -2;}
	if (idx.y >= dimensions.y){ idx.y = -2;}
	if (idx.x < 0){ idx.x = -1;}
	if (idx.y < 0){ idx.y = -1;}
	return idx;
}

sf::Vector2<float> Map::tileIdxToPos(sf::Vector2<int> idx)
{
	//Check for valid idx
	if (idx.x >= dimensions.x || idx.y >= dimensions.y || idx.x < 0 || idx.y < 0) {
		std::cout << "Cannot get map position at index, index out of range..." << std::endl;
		std::cout << "\tIdx: " << idx.x << ", " << idx.y << std::endl;
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

		//LOG_ERROR("Could not load map: " + filename + ", incorrect type...", "");

		return -1;
	}

	//Read texture info
	int nTilesets;  std::string textureName; int textureSize = 16; //the expected size of each subrect in the tile sheet
	inFile >> nTilesets;

	//logger.info("Looking for " + std::to_string(nTilesets) + " tilesets");
	//std::cout << "Looking for " << nTilesets << " tilesets..." << std::endl;
	
	for (int i = 0; i < nTilesets; i++) {
		inFile >> textureName >> textureSize;
		std::pair<std::string, int> set;
		set.first = textureName; set.second = textureSize;
		tilesets.push_back(set);

		std::cout << "\tread set: " << tilesets.back().first << ", " << tilesets.back().second << "x" << tilesets.back().second << std::endl;
	}

	inFile >> tileSize; //The size that each tile will be scaled to

	std::cout << "Tilesize: " << tileSize << std::endl;
		 
	//Read dimensions
	inFile >> dimensions.x >> dimensions.y;
	std::cout << "Dimensions: " << dimensions.x << "x" << dimensions.y << std::endl;

	//Read Tiles
	Tile* t; int sheet; int type; int pass; float cost; float rotation;

	for (unsigned int i = 0; i < dimensions.y; i++) {
		tiles.push_back(new std::vector<Tile*>());
		for (unsigned j = 0; j < dimensions.x; j++)
		{
			//Read attributes
			char c;
			inFile >> c; //discard [

			inFile >> sheet >> type >> pass >> cost >> rotation;

			inFile >> c; //discard ]

			//Set attributes
			t = new Tile();
			t->sheet = sheet;
			t->type = type;
			t->moveCost = cost;
			t->isPassable = pass;
			t->rotation = sf::degrees(rotation);

			
			t->sprite.setTexture(texReg->lookup(tilesets.at(t->sheet).first));
			t->sprite.setFrameSize(tilesets.at(t->sheet).second, tilesets.at(t->sheet).second);
			t->sprite.setRotation(sf::degrees(rotation));
			t->sprite.setFrame(type);

			t->sprite.setSize({ (float)tileSize, (float)tileSize });
			t->sprite.setPosition({ (float)j * tileSize + offset.x, (float)i * tileSize + offset.y });
			t->sprite.setOrigin({ t->sprite.getLocalBounds().size.x / 2.f, t->sprite.getLocalBounds().size.y / 2.f });
			t->pos = { j,i };
			tiles.at(i)->push_back(t);
		}
	}

	//Close file
	inFile.close();

	this->generateGrid();

	std::cout << "Map Loaded..." << std::endl;

	return 1;
}

int Map::writeToFile(std::string filename)
{
	//## Open file
	std::ofstream outFile;
	outFile.open(filename);

	if (!outFile.is_open())
	{
		std::cout << "Could not open file to write: " << filename << std::endl;
		return -1;
	}

	//## Write map data
	//File type
	outFile << 100 << std::endl;

	//Num tilesets
	outFile << (int)tilesets.size() << std::endl;

	//Tilset texture names and sizes
	for (auto s : tilesets)
	{
		outFile << s.first << " " << s.second << std::endl;
	}

	//Tile size
	outFile << tileSize << std::endl;

	//Map Dimensions
	outFile << dimensions.x << " "  << dimensions.y << std::endl;

	//Tile Data
	for (auto i = 0; i < dimensions.y; i++)
	{
		for (auto j = 0; j < dimensions.x; j++)
		{
			outFile << *tiles.at(i)->at(j) << " ";
		}
		outFile << std::endl;
	}

	//## Close file
	outFile.close();
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

void Map::setTileAtIdx(sf::Vector2<unsigned int> idx, Tile fill)
{
	if (idx.x >= dimensions.x || idx.y >= dimensions.y)
	{
		std::cout << "Could not set tile at index: invalid index" << std::endl;
		return;
	}

	delete tiles.at(idx.y)->at(idx.x);
	tiles.at(idx.y)->at(idx.x) = new Tile(fill);
}

bool Map::containsPos(sf::Vector2<float> pos)
{
	if (tiles.empty())
	{
		return false;
	}
	
	float minX = tiles.front()->front()->sprite.getGlobalBounds().position.x;
	float minY = tiles.front()->front()->sprite.getGlobalBounds().position.y;
	float maxX = tiles.back()->back()->sprite.getGlobalBounds().position.x + tiles.back()->back()->sprite.getGlobalBounds().size.x;
	float maxY = tiles.back()->back()->sprite.getGlobalBounds().position.y + tiles.back()->back()->sprite.getGlobalBounds().size.y;

	return (pos.x >= minX && pos.x < maxX) && (pos.y >= minY && pos.y < maxY);
}

bool Map::containsIdx(sf::Vector2<int> idx)
{
	unsigned int x = (unsigned int)idx.x;
	unsigned int y = (unsigned int)idx.y;
	if (x >= 0 && x < dimensions.x && y >= 0 && y < dimensions.y)
	{
		return true;
	}
	return false;
}

int Map::mnhtnDist(sf::Vector2<float> a, sf::Vector2<float> b)
{
	auto idxA = posToTileIdx(a);
	auto idxB = posToTileIdx(b);

	return std::fabs(idxA.x - idxB.x) + std::fabs(idxA.y - idxB.y);
}

void Map::refreshTextures()
{
	for (int i = 0; i < dimensions.y; i++)
	{
		for (int j = 0; j <dimensions.x; j++)
		{
			int sheetIdx = tiles.at(i)->at(j)->sheet;
			int frameSize = tilesets.at(sheetIdx).second;
			tiles.at(i)->at(j)->sprite.setTexture(texReg->lookup(tilesets.at(sheetIdx).first));
			tiles.at(i)->at(j)->sprite.setFrameSize(frameSize, frameSize);
			tiles.at(i)->at(j)->sprite.setFrame(tiles.at(i)->at(j)->type);
		}
	}
}

void Map::refreshTextureAt(int x, int y)
{
	if (x < 0 || x >= dimensions.x || y < 0 || y >= dimensions.y)
	{
		return;
	}

	int sheetIdx = tiles.at(y)->at(x)->sheet;
	int frameSize = tilesets.at(sheetIdx).second;
	tiles.at(y)->at(x)->sprite.setTexture(texReg->lookup(tilesets.at(sheetIdx).first));
	tiles.at(y)->at(x)->sprite.setFrameSize(frameSize, frameSize);
	tiles.at(y)->at(x)->sprite.setFrame(tiles.at(y)->at(x)->type);
}

void Map::modTileSheetAt(unsigned int x, unsigned int y, int sheet)
{
	if (x >= dimensions.x || y >= dimensions.y)
	{
		return;
	}

	tiles.at(y)->at(x)->sheet = sheet;
}

void Map::modTileTypeAt(unsigned int x, unsigned int y, int type)
{
	if (x >= dimensions.x || y >= dimensions.y)
	{
		return;
	}

	tiles.at(y)->at(x)->type = type;
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
