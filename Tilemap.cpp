#include "Tilemap.h"

//#######################################################################################################
//      Tile Layer
//#######################################################################################################

#pragma region TileLayer
bool tmap::TileLayer::validIdx(int x, int y)
{
    if (tiles.empty())
    {
        return false;
    }

    if (x < 0 || x >= tiles.back().size() || y < 0 || y >= tiles.size())
    {
        return false;
    }

    return true;
}

tmap::TileLayer::TileLayer(int width, int height, int tilesize, sf::Texture* texture, int frameSize)
{
    tileSize = tilesize;

    //Allocate tiles
    for (auto i = 0; i < height; i++)
    {
        tiles.push_back(std::vector<Tile*>());
        for (auto j = 0; j < width; j++)
        {
            tiles.at(i).push_back(new Tile());
            Tile* t = tiles.at(i).at(j);
            t->sprite.setTexture(texture);
            t->sprite.setFrameSize((unsigned int)frameSize, (unsigned int)frameSize);
            t->sprite.setFrame(0);
            t->sprite.setSize({(float)tilesize, (float)tilesize}); //set the size of sprite   
            t->sprite.setPosition({ (float)j * tileSize + offset.x, (float)i * tileSize + offset.y });
            t->sprite.setOrigin({ t->sprite.getLocalBounds().size.x / 2.f, t->sprite.getLocalBounds().size.y / 2.f });
            t->coords = {j,i};
        }
    }
}

tmap::TileLayer::TileLayer(int width, int height, int tilesize)
{
    tileSize = tilesize;

    //Allocate tiles
    for (auto i = 0; i < height; i++)
    {
        tiles.push_back(std::vector<Tile*>());
        for (auto j = 0; j < width; j++)
        {
            tiles.at(i).push_back(new Tile());
            Tile* t = tiles.at(i).at(j);
            t->sprite.setSize({ (float)tilesize, (float)tilesize }); //set the size of sprite   
            t->sprite.setPosition({ (float)j * tileSize + offset.x, (float)i * tileSize + offset.y });
            t->sprite.setOrigin({ t->sprite.getLocalBounds().size.x / 2.f, t->sprite.getLocalBounds().size.y / 2.f });
            t->coords = { j,i };
        }
    }
}

tmap::TileLayer::~TileLayer()
{
    //Deallocate tiles
    for (auto i = 0; i < tiles.size(); i++)
    {
        for (auto j = 0; j < tiles.at(i).size(); j++)
        {
            delete tiles.at(i).at(j);
        }
        tiles.at(i).clear();
    }

    tiles.clear();

}

tmap::Tile* tmap::TileLayer::getTileAt(int x, int y)
{
    //Should put some kind of check for valid idx, but brain no work right now, what to return in bad case?
    return tiles.at(y).at(x);
}

void tmap::TileLayer::setTileAt(int x, int y, const Tile& data)
{
    if (!validIdx(x,y))
    {
        return;
    }

    tiles.at(y).at(x)->sheet = data.sheet;
    tiles.at(y).at(x)->type = data.type;
    tiles.at(y).at(x)->isSolid = data.isSolid;

    std::string coordStr = std::to_string(x) + "x" + std::to_string(y);
    dirtyTiles.insert({ coordStr, tiles.at(y).at(x) });
}

void tmap::TileLayer::setTypeAt(int x, int y, int type)
{
    if (!validIdx(x, y))
    {
        return;
    }

    tiles.at(y).at(x)->type = type;
    std::string coordStr = std::to_string(x) + "x" + std::to_string(y);
    dirtyTiles.insert({ coordStr, tiles.at(y).at(x) });
}

void tmap::TileLayer::setSheetAt(int x, int y, int sheet)
{
    if (!validIdx(x, y))
    {
        return;
    }

    tiles.at(y).at(x)->sheet = sheet;
    std::string coordStr = std::to_string(x) + "x" + std::to_string(y);
    dirtyTiles.insert({ coordStr, tiles.at(y).at(x) });
}

void tmap::TileLayer::setSolidAt(int x, int y, int state)
{
    if (!validIdx(x, y))
    {
        return;
    }

    tiles.at(y).at(x)->isSolid = state;
}

void tmap::TileLayer::setTileDataAt(int x, int y, int type, int sheet, int isSolid)
{
    if (!validIdx(x, y))
    {
        return;
    }

    tiles.at(y).at(x)->sheet = sheet;
    tiles.at(y).at(x)->type = type;
    tiles.at(y).at(x)->isSolid = isSolid;

    std::string coordStr = std::to_string(x) + "x" + std::to_string(y);
    dirtyTiles.insert({ coordStr, tiles.at(y).at(x) });
}

void tmap::TileLayer::replaceTileAt(int x, int y, Tile* tile)
{
    if (!validIdx(x, y) || !tile)
    {
        return;
    }

    delete tiles.at(y).at(x);
    tiles.at(y).at(x) = tile;
    tiles.at(y).at(x)->coords = {x,y};
}

void tmap::TileLayer::refreshTiles(TextureRegistry& texReg, std::vector<std::pair<std::string, int>> tilesets, int tilesize)
{
    //Refresh tile texture and sprite data
    for (auto it = dirtyTiles.begin(); it != dirtyTiles.end(); it++)
    {
        Tile* t = (*it).second;
        unsigned int frameSize = (unsigned int)tilesets.at(t->sheet).second;
        t->sprite.setTexture(texReg.lookup(tilesets.at(t->sheet).first));
        t->sprite.setFrameSize(frameSize, frameSize);
        t->sprite.setFrame(t->type);
        t->sprite.setSize({ (float)tilesize , (float)tilesize });

        t->sprite.setSize({ (float)tilesize, (float)tilesize }); //set the size of sprite   
        t->sprite.setPosition({ (float)t->coords.x * tileSize + offset.x, (float)t->coords.y * tileSize + offset.y });
        t->sprite.setOrigin({ t->sprite.getLocalBounds().size.x / 2.f, t->sprite.getLocalBounds().size.y / 2.f });
    }

    //Clear List
    dirtyTiles.clear();
}

std::optional<sf::Rect<float>> tmap::TileLayer::intersectionAt(int x, int y, sf::FloatRect rect)
{
    if (!validIdx(x, y))
    {
        return std::optional<sf::Rect<float>>(); //should return empty optional
    }

    return rect.findIntersection(tiles.at(y).at(x)->sprite.getGlobalBounds());
}

bool tmap::TileLayer::containsAt(int x, int y, sf::Vector2<float> pos)
{
    if (!validIdx(x, y))
    {
        return false;
    }

    if (!tiles.at(y).at(x)->sprite.getGlobalBounds().contains(pos))
    {
        return false;
    }

    return true;
}


void tmap::TileLayer::setOffset(sf::Vector2<float> newOffset)
{
    sf::Vector2f dif = newOffset - offset;
    offset = newOffset;

    this->move(dif);
}

void tmap::TileLayer::move(sf::Vector2<float> amt)
{
    offset += amt;

    for (auto i = 0; i < tiles.size(); i++)
    {
        for (auto j = 0; j < tiles.at(i).size(); j++)
        {
            tiles.at(i).at(j)->sprite.move(amt);
        }
    }
}

void tmap::TileLayer::draw(sf::RenderWindow& win)
{
    for (auto i = 0; i < tiles.size(); i++)
    {
        for (auto j = 0; j < tiles.at(i).size(); j++)
        {
            tiles.at(i).at(j)->sprite.draw(win);
        }
    }
}
#pragma endregion


//#######################################################################################################
//      Tile Map
//#######################################################################################################

#pragma region TileMap

void tmap::Tilemap::clearLayers()
{
    //De-allocate layer memory
    for (auto i = 0; i < layers.size(); i++)
    {
        delete layers.at(i);
    }

    //Empty layer vector
    layers.clear();
}

int tmap::Tilemap::readLayer(std::ifstream& inFile)
{
    std::cout << "Reading Layer" << std::endl;

    if (!inFile.is_open())
    {
        std::cout << "Layer Read Failed: invalid input stream" << std::endl;
        return -1;
    }

    int sheet; int type; int solid;

    TileLayer* layer = new TileLayer(dimensions.x, dimensions.y, tileSize);
    layer->setOffset(offset);
    layers.push_back(layer);

    for (int i = 0; i < dimensions.y; i++)
    {
        for (int j = 0; j < dimensions.x; j++)
        {
            //Read attributes
            char c;
            inFile >> c; //discard [

            inFile >> sheet >> type >> solid;

            inFile >> c; //discard ]

            //Set attributes
            Tile t;
            t.sheet = sheet;
            t.type = type;
            t.isSolid = solid;

            layer->setTileAt(j, i, t);    
        }
        layer->refreshTiles(*texReg, tilesets, tileSize);
    }

    std::cout << "Layer Read Successful" << std::endl;

    return 1;
}

int tmap::Tilemap::writeLayer(std::ofstream& outFile, int layer)
{
    if (!outFile.is_open())
    {
        return -1;
    }

    //Write tiles
    for (auto i = 0; i < dimensions.y; i++)
    {
        for (auto j = 0; j < dimensions.x; j++)
        {
            //[sheet type isSolid] ...
            Tile t = *layers.at(layer)->getTileAt(j,i);
            outFile << "[" << t.sheet << " " << t.type << " " << std::noboolalpha << t.isSolid << "]";
        }
        outFile << std::endl;
    }



    return 1;
}

bool tmap::Tilemap::validIdx(int x, int y, int z)
{
    if (x < 0 || x >= dimensions.x) { return false; }
    if (y < 0 || y >= dimensions.y) { return false; }
    if (z < 0 || z >= dimensions.z) { return false; }

    return true;
}

void tmap::Tilemap::generateGrid()
{
    grid.clear();

    //Horizontal lines
    for (auto i = 0; i <= dimensions.y; i++)
    {
        sf::Vector2<float> posA = { offset.x - (tileSize / 2), i * tileSize + offset.y - (tileSize / 2) };
        sf::Vector2<float> posB = { dimensions.x * tileSize + offset.x - (tileSize / 2), i * tileSize + offset.y - (tileSize / 2) };
        grid.push_back({ posA, gridColor,{0,0} });
        grid.push_back({ posB, gridColor, {0,0} });
    }

    //Vertical lines
    for (auto i = 0; i <= dimensions.x; i++)
    {
        sf::Vector2<float> posA = { i * tileSize + offset.x - (tileSize / 2), offset.y - (tileSize / 2) };
        sf::Vector2<float> posB = { i * tileSize + offset.x - (tileSize / 2), dimensions.y * tileSize + offset.y - (tileSize / 2) };
        grid.push_back({ posA, gridColor,{0,0} });
        grid.push_back({ posB, gridColor, {0,0} });
    }
}

tmap::Tilemap::Tilemap(int width, int height, int layers, int tileSize, TextureRegistry* textureRegistry, std::string texName, int texSize)
{
    this->dimensions = {width, height, layers};
    this->tileSize = tileSize;
    this->texReg = textureRegistry;

    tilesets.push_back({texName, texSize});
    sf::Texture* initTexture = texReg->lookup(texName);

    for (auto i = 0; i < layers; i++)
    {
        TileLayer* l = new TileLayer(width, height, tileSize, initTexture, texSize);
        this->layers.push_back(l);
    }
}

tmap::Tilemap::Tilemap(TextureRegistry* textureRegistry)
{
    texReg = textureRegistry;
}

tmap::Tilemap::~Tilemap()
{
    this->clearLayers();
}

int tmap::Tilemap::loadFromFile(std::string filename)
{
    std::cout << "LOADING MAP: " << filename << std::endl;

    this->clearLayers();
    dimensions = {0,0,0};
    tilesets.clear();

    //## Open File
    std::ifstream inFile;
    inFile.open(filename);
    if (!inFile.is_open())
    {
        std::cout << "Could not open map file: [" << filename << "] for read..." << std::endl;
        return -1;
    }

    //## Read Header
    //filetype
    int fType;
    inFile >> fType;
    if (fType != 200)
    {
        std::cout << "Could not load map: " << filename << ", incorrect type..." << std::endl;

        return -1;
    }

    //texture info
    int nTilesets;  std::string textureName; int textureSize = 16; //the expected size of each subrect in the tile sheet
    inFile >> nTilesets;


    for (int i = 0; i < nTilesets; i++)
    {
        inFile >> textureName >> textureSize;
        std::pair<std::string, int> set;
        set.first = textureName; set.second = textureSize;
        tilesets.push_back(set);

        std::cout << "\tread set: " << tilesets.back().first << ", " << tilesets.back().second << "x" << tilesets.back().second << std::endl;
    }

    inFile >> tileSize; //The size that each tile will be scaled to

    std::cout << "Tilesize: " << tileSize << std::endl;

    //Read dimensions
    inFile >> dimensions.x >> dimensions.y >> dimensions.z;
    std::cout << "Dimensions: " << dimensions.x << "x" << dimensions.y << "x" << dimensions.z << std::endl;

    //## Read Layers
    for (int i = 0; i < dimensions.z; i++)
    {
        //For each layer
        this->readLayer(inFile);
        std::cout << i << " ";
    }
    std::cout << std::endl;
    std::cout << "Layer Read Complete." << std::endl;

    //## Close File
    inFile.close();

    std::cout << "LOAD COMPLETE" << std::endl;
    return 1;

    
}

int tmap::Tilemap::writeToFile(std::string filename)
{
    std::cout << "WRITING MAP FILE..." << std::endl;
    //## Open File
    std::ofstream outFile;
    outFile.open(filename);

    if (!outFile.is_open())
    {
        std::cout << "Could not open map file: [" << filename << "] for write..." << std::endl;
        return -1;
    }

    //## Write Data
    //Write header
    outFile << "200" << std::endl;

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
    outFile << dimensions.x << " " << dimensions.y << " " << dimensions.z << std::endl;

    //Write layer
    for (auto i = 0; i < dimensions.z; i++)
    {
        this->writeLayer(outFile, i);
    }

    //## Close File
    outFile.close();

    std::cout << "WRITE COMPLETE" << std::endl;
    return 1;
}

void tmap::Tilemap::modTileAt(int x, int y, int z, Tile data)
{
    if (!validIdx(x, y, z))
    {
        return;
    }

    layers.at(z)->setTileAt(x,y,data);
    layers.at(z)->refreshTiles(*texReg,tilesets,tileSize);
}

tmap::Tile* tmap::Tilemap::getTileAt(int x, int y, int z)
{
    if (!validIdx(x,y,z))
    {
        return nullptr;
    }

    return layers.at(z)->getTileAt(x, y);
}

sf::Vector2<int> tmap::Tilemap::getCoords(sf::Vector2<float> pos)
{
    //Account for centered origin
    pos.x += (float)tileSize / 2.f;
    pos.y += (float)tileSize / 2.f;

    pos.x -= offset.x;
    pos.y -= offset.y;

    //Get index
    sf::Vector2<int> idx = { (int)std::floor(pos.x / tileSize),(int)std::floor(pos.y / tileSize) };

    //return 0 for axis if off map to top or left, return -2 if off map to bottom or right (this is insane, but itll work, just trust me bro)
    if (idx.x >= dimensions.x) { idx.x = -2; }
    if (idx.y >= dimensions.y) { idx.y = -2; }
    if (idx.x < 0) { idx.x = -1; }
    if (idx.y < 0) { idx.y = -1; }
    return idx;
}

bool tmap::Tilemap::containsCoord(sf::Vector2<int> coord)
{
    return validIdx(coord.x, coord.y, 0);
}

bool tmap::Tilemap::containsPos(sf::Vector2<float> pos)
{
    if (layers.empty() || dimensions.y <= 0 || dimensions.x <= 0)
    {
        return false;
    }

    float minX = layers.at(0)->getTileAt(0,0)->sprite.getGlobalBounds().position.x;
    float minY = layers.at(0)->getTileAt(0, 0)->sprite.getGlobalBounds().position.y;
    float maxX = layers.at(0)->getTileAt(dimensions.x - 1, dimensions.y - 1)->sprite.getGlobalBounds().position.x + layers.at(0)->getTileAt(dimensions.x - 1, dimensions.y - 1)->sprite.getGlobalBounds().size.x;
    float maxY = layers.at(0)->getTileAt(dimensions.x - 1, dimensions.y - 1)->sprite.getGlobalBounds().position.y + layers.at(0)->getTileAt(dimensions.x - 1, dimensions.y - 1)->sprite.getGlobalBounds().size.y;

    return (pos.x >= minX && pos.x < maxX) && (pos.y >= minY && pos.y < maxY);
}

void tmap::Tilemap::printTile(int x, int y, int z)
{
    if (!validIdx(x,y,z))
    {
        std::cout << "[Invalid Idx]";
        return;
    }
    Tile* tile = layers.at(z)->getTileAt(x, y);

    std::cout << "[" << tile->sheet << " " << tile->type << " " << tile->isSolid << "]";
}

sf::Vector2<float> tmap::Tilemap::getPos(sf::Vector2<int> coord)
{
    //Check for valid idx
    if (!validIdx(coord.x, coord.y, 0))
    {
        std::cout << "Cannot get map position at coordinate, index out of range..." << std::endl;
        std::cout << "Coordinate: " << coord.x << ", " << coord.y << std::endl;
        return { -1.f,-1.f };
    }

    //If idx valid return position of tile at idx
    return layers.at(0)->getTileAt(coord.x, coord.y)->sprite.getPosition();
}


void tmap::Tilemap::poll(sf::RenderWindow& win, std::optional<sf::Event> event)
{

}

void tmap::Tilemap::update(float dt)
{

}


void tmap::Tilemap::draw(sf::RenderWindow& win)
{
    //Draw Tiles
    for (auto l : layers)
    {
        l->draw(win);
    }

    //Draw Grid
    if (doDrawGrid)
    {
        for (auto i = 0; i < grid.size(); i += 2)
        {
            sf::Vertex line[2] = { grid.at(i),grid.at(i + 1) };
            win.draw(line, 2, sf::PrimitiveType::Lines);
        }
    }
}

#pragma endregion