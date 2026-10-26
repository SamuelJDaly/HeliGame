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
            t->sprite.setSize({(float)tilesize, (float)tilesize}); //set the size of sprite   
            t->sprite.setPosition({ (float)j * tileSize + offset.x, (float)i * tileSize + offset.y });
            t->sprite.setOrigin({ t->sprite.getLocalBounds().size.x / 2.f, t->sprite.getLocalBounds().size.y / 2.f });
            t->coords = {j,i};
        }
    }
}

tmap::TileLayer::~TileLayer()
{
    //Deallocate tiles
    for (auto i = 0; i < tiles.size(); i++)
    {
        tiles.push_back(std::vector<Tile*>());
        for (auto j = 0; j < tiles.at(i).size(); j++)
        {
            delete tiles.at(i).at(j);
        }
        tiles.at(i).clear();
    }

    tiles.clear();

}

tmap::Tile tmap::TileLayer::getTileAt(int x, int y)
{
    //Should put some kind of check for valid idx, but brain no work right now, what to return in bad case?
    return *tiles.at(y).at(x);
}

void tmap::TileLayer::setTileAt(int x, int y, const Tile& data)
{
    if (!validIdx(x,y))
    {
        return;
    }

    //Is this actually a good use case for copy constructor?
    delete tiles.at(y).at(x);
    tiles.at(y).at(x) = new Tile(data);
    tiles.at(y).at(x)->coords = {x,y};
}

void tmap::TileLayer::setTypeAt(int x, int y, int type, TextureRegistry& texReg, std::vector<std::pair<std::string, int>> tilesets, int tilesize)
{
    if (!validIdx(x, y))
    {
        return;
    }

    int sheet = tiles.at(y).at(x)->sheet;

    tiles.at(y).at(x)->type = type;
    tiles.at(y).at(x)->sprite.setTexture(texReg.lookup(tilesets.at(sheet).first)); //Set the texture sheet
    tiles.at(y).at(x)->sprite.setFrameSize((unsigned int)tilesets.at(sheet).second, (unsigned int)tilesets.at(sheet).second);
    tiles.at(y).at(x)->sprite.setFrame(type); //Set the texture rect
    tiles.at(y).at(x)->sprite.setSize({(float)tilesize, (float)tilesize}); //set the size of sprite
}

void tmap::TileLayer::setSheetAt(int x, int y, int sheet, TextureRegistry& texReg, std::vector<std::pair<std::string, int>> tilesets, int tilesize)
{
    if (!validIdx(x, y))
    {
        return;
    }

    int type = tiles.at(y).at(x)->type;

    tiles.at(y).at(x)->sheet = sheet;
    tiles.at(y).at(x)->sprite.setTexture(texReg.lookup(tilesets.at(sheet).first)); //Set the texture sheet
    tiles.at(y).at(x)->sprite.setFrameSize((unsigned int)tilesets.at(sheet).second, (unsigned int)tilesets.at(sheet).second);
    tiles.at(y).at(x)->sprite.setFrame(type); //Set the texture rect
    tiles.at(y).at(x)->sprite.setSize({ (float)tilesize , (float)tilesize }); //set the size of sprite
}

void tmap::TileLayer::setSolidAt(int x, int y, int state)
{
    if (!validIdx(x, y))
    {
        return;
    }

    tiles.at(y).at(x)->isSolid = state;
}

void tmap::TileLayer::setTileDataAt(int x, int y, int type, int sheet, int isSolid, TextureRegistry& texReg, std::vector<std::pair<std::string, int>> tilesets, int tilesize)
{
    if (!validIdx(x, y))
    {
        return;
    }


    tiles.at(y).at(x)->sheet = sheet;
    tiles.at(y).at(x)->type = type;
    tiles.at(y).at(x)->isSolid = isSolid;
    tiles.at(y).at(x)->sprite.setTexture(texReg.lookup(tilesets.at(sheet).first)); //Set the texture sheet
    tiles.at(y).at(x)->sprite.setFrameSize((unsigned int)tilesets.at(sheet).second,(unsigned int)tilesets.at(sheet).second);
    tiles.at(y).at(x)->sprite.setFrame(type); //Set the texture rect
    tiles.at(y).at(x)->sprite.setSize({ (float)tilesize , (float)tilesize }); //set the size of sprite
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
    for (auto l : layers)
    {
        delete l;
    }

    //Empty layer vector
    layers.clear();
}

int tmap::Tilemap::readLayer(std::ifstream& inFile)
{
    if (!inFile.is_open())
    {
        return -1;
    }

    int sheet; int type; int solid;

    for (unsigned int i = 0; i < dimensions.y; i++)
    {
        TileLayer* layer = new TileLayer(dimensions.x, dimensions.y,tileSize);
        layers.push_back(layer);
        for (unsigned j = 0; j < dimensions.x; j++)
        {
            //Read attributes
            char c;
            inFile >> c; //discard [

            inFile >> type >> sheet >> solid;

            inFile >> c; //discard ]

            //Set attributes
            Tile t;
            t.sheet = sheet;
            t.type = type;
            t.isSolid = solid;

            t.sprite.setSize({ (float)tileSize, (float)tileSize });
            t.sprite.setPosition({ (float)j * tileSize + offset.x, (float)i * tileSize + offset.y });
            t.sprite.setOrigin({ t.sprite.getLocalBounds().size.x / 2.f, t.sprite.getLocalBounds().size.y / 2.f });
            t.coords = { j,i };

            layer->setTileAt(j, i, t);            
        }
    }

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
            Tile t = layers.at(layer)->getTileAt(j,i);
            outFile << "[" << t.sheet << " " << t.type << " " << std::noboolalpha << t.isSolid << "]";
        }
        outFile << std::endl;
    }



    return 1;
}

tmap::Tilemap::Tilemap(int width, int height, int layers, int tileSize, TextureRegistry* textureRegistry)
{
    this->dimensions = {width, height, layers};
    this->tileSize = tileSize;
    this->texReg = textureRegistry;

    for (auto i = 0; i < layers; i++)
    {
        TileLayer* l = new TileLayer(width, height, tileSize);
        this->layers.push_back(l);
    }
}

tmap::Tilemap::~Tilemap()
{
    this->clearLayers();
}

int tmap::Tilemap::loadFromFile(std::string filename)
{
    std::cout << "LOADING MAP: " << filename << std::endl;

    this->clearLayers();

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
    for (auto i = 0; i < dimensions.z; i++)
    {
        //For each layer
        this->readLayer(inFile);
    }

    //## Close File
    inFile.close();
    return 1;

    std::cout << "LOAD COMPLETE" << std::endl;
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

#pragma endregion