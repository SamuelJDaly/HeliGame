#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <filesystem>

#include "portable-file-dialogs.h"

#include "Gamestate.h"
#include "Tilemap.h"

/*
NOTES:
	- Revist add tileset action to make sure it is robust against commands executed in the wrong order (has memory leak right now I think)
	  Also make sure that the state swap logic makes sense.

	- In the absence of an alternative plan, I will keep the current tileset addition action logic for now. That said, duplicating the map for every single
	  tileset addition action is insane, and there has to be a less wasteful way to acomplish the same end. Revisit this later.

	- Make sure to update the currIdx and lastIdx in the paiting logic to reset when changing which map is being edited (ie new map and open map)

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
	tmap::Tilemap* target;
	std::vector<sf::Vector3<int>> positions;
	std::vector<tmap::Tile> originalState;
	tmap::Tile selection;

	//## Util


public:
	//## Constructor and Destructor
	ActionPaint(tmap::Tilemap* target, std::vector<sf::Vector3<int>> positions, tmap::Tile selection);
	~ActionPaint();

	//## Primary Functions
	int execute();
	int undo();
	inline void setOriginalState(std::vector<tmap::Tile> originalState) { this->originalState = originalState; }
};

//class ActionRemoveTileset : public EditorAction
//{
//private:
//	//## Data
//	tmap::Tilemap* target;
//	tmap::Tilemap* ogState;
//
//	//## Util
//
//
//public:
//	//## Constructor and Destructor
//	ActionRemoveTileset(tmap::Tilemap* target, int idx);
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
	tmap::Tilemap* target;
	tmap::Tilemap* ogState = nullptr;
	bool isRestored = false;

	std::pair<std::string, int> set = {"",-1};

	//## Util


public:
	//## Constructor and Destructor
	inline ActionAddTileset(tmap::Tilemap* target, std::pair<std::string, int> set) { this->target = target; this->set = set; }
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
	//Camera
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

	//Map Control
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
	int newMapTilesize = 32;

	tmap::Tilemap* map;

	//Misc
	bool doShowDemoGui = false;
	float topBarHeight = 20.f;
	std::vector<sf::Color> gridColorOptions = { sf::Color::Black, sf::Color::White, sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow };
	std::vector<std::string> gridColorNames = { "Black", "White", "Red", "Green", "Blue", "Yeller" };
	bool showMapGrid = true;

	//Tile Editor
	bool doShowTileEditor = false;
	tmap::Tile brushFill;
	std::vector<std::vector<bool>> brushShape = { {true} };
	std::vector<sf::Vector3<int>> paintedTiles;
	std::vector<tmap::Tile> originalStates;
	bool isPaintToolSelected = false;
	bool isPainting = false;
	bool isBrushDrawn = false;
	int currLayer = 0;
	
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
	std::vector<sf::Vector3<int>> getBrushPositions(sf::Vector2<int> pos);
	void setBrushCircle(float radius);
	void setBrushSquare(int width, int height);

	int createMap(unsigned int width, unsigned int height, int tileSize);
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

