#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <filesystem>

#include "portable-file-dialogs.h"

#include "Gamestate.h"
#include "Map.h"

/*
TODO:
	- Revist add tileset action to make sure it is robust against commands executed in the wrong order (has memory leak right now I think)
	  Also make sure that the state swap logic makes sense.

*/


//##################################################################################################################
//		ACTIONS
//##################################################################################################################
class EditorAction
{
protected:
	//## Common Data
	bool isUndoable = false;

public:
	//## Common Functions
	inline bool getUndoable() { return isUndoable; }

	virtual int execute() = 0;
	virtual int undo() = 0;
};

class ActionPaint : public EditorAction
{
private:
	//## Data
	Map* target;
	std::vector<sf::Vector2<int>> positions;
	std::vector<Tile> originalState;
	Tile selection;

	//## Util


public:
	//## Constructor and Destructor
	ActionPaint(Map* target, std::vector<sf::Vector2<int>> positions, Tile selection);
	~ActionPaint();

	//## Primary Functions
	int execute();
	int undo();
	inline void setOriginalState(std::vector<Tile> originalState) { this->originalState = originalState; }
};

//class ActionRemoveTileset : public EditorAction
//{
//private:
//	//## Data
//	Map* target;
//	Map* ogState;
//
//	//## Util
//
//
//public:
//	//## Constructor and Destructor
//	ActionRemoveTileset(Map* target, int idx);
//	~ActionRemoveTileset();
//
//	//## Primary Functions
//	int execute();
//	int undo();
//};

class ActionAddTileset : public EditorAction
{
private:
	//## Data
	Map* target;
	Map* ogState = nullptr;
	bool isRestored = false;

	std::pair<std::string, int> set = {"",-1};

	//## Util


public:
	//## Constructor and Destructor
	inline ActionAddTileset(Map* target, std::pair<std::string, int> set) { this->target = target; this->set = set; }
	inline ~ActionAddTileset() { if (ogState) { delete ogState; } }

	//## Primary Functions
	int execute();
	int undo();
};



//##################################################################################################################
//		EDITOR STATE
//##################################################################################################################
class EditorState : public Gamestate
{
private:
	//## Data
	sf::View mapView;
	sf::Vector2<float> cameraOffset = { 0.f,0.f };
	float panSpeed = 850.f;
	float maxZoom = 10.5f;
	float minZoom = 0.1f;
	float zoomSpeed = .06f;
	float zoomStep = .1f;
	float currZoom = 1.f;
	bool canZoom = true;
	float panSpeedMult = 1.2f;
	bool isPan = false;
	sf::Vector2f panStart = { 0.f,0.f };


	bool isUnsaved = false;
	int maxUndoStates = 3;
	std::stack<EditorAction*> undoStack;
	std::stack<EditorAction*> redoStack;
	std::string filepath = "resource/test.txt";
	std::string defaultMapFilepath = "resource/"; //Need to look into os filepath handling to make sure this is robust enough
	bool doShowGuiNewMap = false;
	char newMapName[512] = "";
	int newMapWidth = 10;
	int newMapHeight = 10;

	Map* map;

	bool doShowDemoGui = false;
	float topBarHeight = 20.f;

	bool doShowTileEditor = false;
	Tile brushFill;
	std::vector<std::vector<bool>> brushShape = { {true} };
	std::vector<sf::Vector2<int>> paintedTiles;
	std::vector<Tile> originalStates;
	bool isPaintToolSelected = false;
	bool isPainting = false;
	bool isBrushDrawn = false;
	bool showMapGrid = true;
	std::vector<sf::Color> gridColorOptions = { sf::Color::Black, sf::Color::White, sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow };
	std::vector<std::string> gridColorNames = {"Black", "White", "Red", "Green", "Blue", "Yeller"};
	int tilsetSelIdx = 0;
	int tileSelIdx = -1;
	sf::Font font;
	sf::Text textBrushSheet = sf::Text(font);
	sf::Text textBrushType = sf::Text(font);

	ImVec2 tsAddPos = ImVec2(0,0);
	bool doShowTilesetAdd = false;
	int tsAddTexSize = 32;
	char tilesetTexturePath[512] = ""; //Filepath to texture
	char tilesetTextureName[512] = ""; //Handle in texture registry
	
	sf::Vector2<int> lastIdx = { -1,-1 };
	sf::Vector2<int> currIdx = { -1,-1 };

	//## Util
	void init();
	bool isBelowMap(sf::Vector2<float> pos);
	bool isRightOfMap(sf::Vector2<float> pos);

	void moveCamera(sf::Vector2<float> offset);
	void updateCamera(float dt);
	void zoomCamera(float zoom);

	void drawBrush(sf::RenderWindow &win);
	std::vector<sf::Vector2<int>> getBrushPositions(sf::Vector2<int> pos);
	void setBrushCircle(float radius);
	void setBrushSquare(int width, int height);

	int createMap(unsigned int width, unsigned int height);
	int saveMap();
	int openMap();
	void exitEditor();

	void undo();
	void redo();
	void clearUndoStack();
	void clearRedoStack();

	//Gui
	void showMenuBar_File();
	void ShowMenuBar_Options();
	void showMenuBar_Edit();
	void showMenuBar_Window();
	void showGuiMenuBar();
	void showGuiNewMap(bool* pOpen);
	void showGuiTileEditor(bool *pOpen);
	void showGuiTilesetAdd(bool* pOpen);
public:
	//## Constructor and Destructor
	EditorState(TextureRegistry* textureRegistry, sf::RenderWindow* window);
	~EditorState();

	//## Primary Functions
	void update(float dt);
	void poll(sf::RenderWindow& win, std::optional<sf::Event> event);
	void draw(sf::RenderWindow& win);
};

