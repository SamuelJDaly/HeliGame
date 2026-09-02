#include "TextureRegistry.h"



//###################	UTIL	#######################################################
std::vector<std::string> split(std::string str, char delim) {
    std::vector<std::string> res;
    const char* cStr = str.c_str();

    do
    {
        const char* begin = cStr;

        while (*cStr != delim && *cStr)
            cStr++;

        res.push_back(std::string(begin, cStr));
    } while (0 != *cStr++);

    return res;
}

//###################	TEXTURE HANDLER	###############################################

TextureRegistry::TextureRegistry()
{
    //Set up default texture
    sf::Image img;
    img.resize({ 1, 1 }, sf::Color::Magenta);
    if (!defaultTexture.loadFromImage(img)) {
        std::cout << "Could not load default texture!" << std::endl;
    }
}

TextureRegistry::~TextureRegistry()
{
}

sf::Texture* TextureRegistry::lookup(std::string key)
{

    auto it = textures.find(key);

    if (it == textures.end()) {
        std::cout << "Texture not found: [" << key << "]" << std::endl;
        return nullptr;
    }
    else {
        return &textures.at(key);
    }


    return nullptr;
}

void TextureRegistry::fLoad(std::string filepath)
{
    /*
        This function reads in a manifest of textures in the form "name textureFile".
        The advantage of this is that the availible textures are modifiable at runtime rather than compile time.
    */

    //### Vars
    std::ifstream inFile;

    //### Open file
    inFile.open(filepath);

    if (!inFile.is_open()) {
        std::cout << "Could not open texture manifest: " << filepath << std::endl;
        return;
    }

    //### Read File
    std::string currLine = "";
    int  line = 0;

    while (std::getline(inFile, currLine)) {
        std::vector<std::string> tokens = split(currLine, ' ');

        if (tokens.size() != 2) {
            std::cout << "Malformed manifest entry on line: " << line << std::endl;
        }

        sf::Texture entry;

        if (!entry.loadFromFile(tokens.at(1))) {
            std::cout << "Could not load texture: " << tokens.at(1) << std::endl;
        }
        else {
            textures.insert({ tokens.at(0), entry });
            if (isVerbose) {
                std::cout << "Texture Loaded: " << tokens.at(0) << ": " << tokens.at(1) << std::endl;
            }
        }

        line++;
    }


    //### Close file
    inFile.close();
}

void TextureRegistry::addTexture(std::string key, std::string filepath)
{
    sf::Texture added;

    if (!added.loadFromFile(filepath)) {
        std::cout << "Could not add texture: " << filepath << std::endl;
        return;
    }

    textures.insert({ key, added });
}

void TextureRegistry::setVerbose(bool state)
{
    isVerbose = state;
}
