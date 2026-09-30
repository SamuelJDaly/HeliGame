#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <stack>

#include "portable-file-dialogs.h"

#include "Gamestate.h"
#include "Map.h"


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
	float maxZoom = 1.5f;
	float minZoom = 0.1f;
	float zoomSpeed = .06f;
	float zoomStep = .1f;
	float currZoom = 1.f;

	bool isUnsaved = false;
	int maxUndoStates = 3;
	std::stack<EditorAction*> undoStack;
	std::stack<EditorAction*> redoStack;
	std::string filepath = "resource/test.txt";
	std::string defaultMapFilepath = "resource/"; //Need to look into os filepath handling to make sure this is robust enough

	Map* map;

	bool doShowDemoGui = false;
	float topBarHeight = 20.f;

	bool doShowTilePallete = false;
	Tile brushFill;
	std::vector<sf::Vector2<int>> paintedTiles;
	std::vector<Tile> originalStates;
	bool isPaintToolSelected = true;
	bool isPainting = false;
	bool isBrushDrawn = false;
	
	sf::Vector2<int> lastIdx = { -1,-1 };
	sf::Vector2<int> currIdx = { -1,-1 };

	//## Util
	void init();
	bool isBelowMap(sf::Vector2<float> pos);
	bool isRightOfMap(sf::Vector2<float> pos);

	void moveCamera(sf::Vector2<float> offset);
	void updateCamera(float dt);

	void drawBrush(sf::RenderWindow &win);

	int createMap();
	int saveMap();
	int openMap();

	void undo();
	void redo();
	void clearUndoStack();
	void clearRedoStack();

	//Gui
	void showMenuBar_File();
	void showMenuBar_Edit();
	void showMenuBar_Window();
	void showGuiMenuBar();
	void showGuiPalleteTool();
public:
	//## Constructor and Destructor
	EditorState(TextureRegistry* textureRegistry, sf::RenderWindow* window);
	~EditorState();

	//## Primary Functions
	void update(float dt);
	void poll(sf::RenderWindow& win, std::optional<sf::Event> event);
	void draw(sf::RenderWindow& win);
};

