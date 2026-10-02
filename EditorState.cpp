#include "EditorState.h"

//#########################################################################################################################
//		ACTIONS
//#########################################################################################################################

#pragma region ACTIONS

#pragma region Paint Action
ActionPaint::ActionPaint(Map* target, std::vector<sf::Vector2<int>> positions, Tile selection)
{
	isUndoable = true;
	this->target = target;
	this->positions = positions;
	this->selection = selection;

}

ActionPaint::~ActionPaint()
{
}

int ActionPaint::execute()
{
	if (!target)
	{
		return 0;
	}

	if (selection.type < 0)
	{
		return 0;
	}
	

	//Loop Through and replace tiles
	for (int i = 0; i < positions.size(); i++)
	{
		sf::Vector2<int> idx = positions.at(i);
		//Check for valid index
		if (idx.x >= 0 && idx.y >= 0 && idx.x < target->getDimensions().x && idx.y < target->getDimensions().y)
		{
			Tile* src = target->tileAtIdx(idx.x, idx.y);
			Tile og = Tile(*src);

			target->modTileSheetAt(idx.x, idx.y, selection.sheet);
			target->modTileTypeAt(idx.x, idx.y, selection.type);
			target->refreshTextureAt(idx.x, idx.y);

			originalState.push_back(og);
		}
	}

	return 0;
}
int ActionPaint::undo()
{
	if (positions.size() != originalState.size())
	{
		return 0;
	}

	for (int i = positions.size() - 1; i >= 0; i--)
	{
		sf::Vector2<unsigned int> idx = { (unsigned int)positions.at(i).x, (unsigned int)positions.at(i).y};
		//Check for valid index
		if (idx.x < target->getDimensions().x && idx.y < target->getDimensions().y)
		{
			target->modTileSheetAt(idx.x, idx.y, originalState.at(i).sheet);
			target->modTileTypeAt(idx.x, idx.y, originalState.at(i).type);
			target->refreshTextureAt(idx.x,idx.y);
		}
	}

	originalState.clear();

	return 0;
}
#pragma endregion

#pragma region AddTileset

int ActionAddTileset::execute()
{
	if (isRestored)
	{
		//Then has been undone, do redo logic
		//Swap the states
		Map* temp = ogState;
		ogState = target;
		target = temp;
		isRestored = false;
	}
	else
	{
		//Then has not been undone, do initial logic
		//Store og state
		ogState = new Map(*target);

		//Modify the current state
		target->addTileset(set);
	}

	return 1;
}


int ActionAddTileset::undo()
{
	if (isRestored)
	{
		return 0;
	}

	isRestored = true;

	//Swap the states back
	Map* temp = target;
	target = ogState;
	ogState = temp;

	return 1;
}

#pragma endregion

#pragma endregion


//#########################################################################################################################
//		EDITOR STATE
//#########################################################################################################################

void EditorState::init()
{
	//Start with empty 10x10 map
	map = new Map(texReg, 32, {10,10}, "tileset_0", 32);
	map->setVeiw(&mapView);
	map->setGridDraw(true);
	map->setGridColor(sf::Color::White);

	//Test
	brushFill.sheet = 0;
	brushFill.type = 1;

	this->moveCamera({-100.f,-100.f});


	if (!font.openFromFile("resource/font/jmhtype.ttf"))
	{
		std::cout << "could not load font!" << std::endl;
		return;
	}

	textBrushSheet.setFont(font);
	textBrushSheet.setCharacterSize(12);
	textBrushSheet.setFillColor(sf::Color::White);
	textBrushSheet.setPosition({ 5.f, (float)win->getSize().y - 60 });

	textBrushType.setFont(font);
	textBrushType.setCharacterSize(12);
	textBrushType.setFillColor(sf::Color::White);
	textBrushType.setPosition({ 5.f, (float)win->getSize().y - 40 });

	this->setBrushCircle(2.f);
	
}

bool EditorState::isBelowMap(sf::Vector2<float> pos)
{
	float mapBottom = map->getDimensions().y * map->getTileSize();
	if (pos.y > mapBottom)
	{
		return true;
	}
	return false;
}

bool EditorState::isRightOfMap(sf::Vector2<float> pos)
{
	float mapRight = map->getDimensions().x * map->getTileSize();
	if (pos.x > mapRight)
	{
		return true;
	}
	return false;
}

void EditorState::moveCamera(sf::Vector2<float> offset)
{
	cameraOffset += offset;
	mapView.move(offset);
}

void EditorState::updateCamera(float dt)
{
	sf::Vector2<int> mousePos = sf::Mouse::getPosition(*this->win);

	//Keyboard Pan
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
	{
		this->moveCamera({ 0, -1.f * panSpeed * currZoom * dt });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
	{
		this->moveCamera({ 0, panSpeed * currZoom * dt });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
	{
		this->moveCamera({ -1.f * panSpeed * currZoom * dt, 0 });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
	{
		this->moveCamera({ panSpeed * currZoom * dt, 0 });
	}

	//Mouse pan
	if (isPan)
	{
		sf::Vector2<float> panAmt = panStart - sf::Vector2<float>((float)mousePos.x, (float)mousePos.y);
		panAmt *= (panSpeedMult * currZoom);
		this->moveCamera(panAmt);
		panStart = sf::Vector2<float>((float)mousePos.x, (float)mousePos.y);
	}
}

void EditorState::zoomCamera(float zoom)
{
	if (zoom < maxZoom && zoom >= minZoom)
	{

		mapView.zoom(1 / currZoom);
		currZoom = zoom;
		mapView.zoom(currZoom);
	}
}

void EditorState::drawBrush(sf::RenderWindow& win)
{
	sf::Vector2<int> mousePos = sf::Mouse::getPosition(win);
	win.setView(mapView);
	sf::Vector2<float> mapViewMousePos = win.mapPixelToCoords(mousePos);
	sf::Vector2<int> mapIdx = map->posToTileIdx(mapViewMousePos);


	std::vector<sf::Vector2<int>> positions = this->getBrushPositions(mapIdx);


	if (mapIdx.x < 0 || mapIdx.y < 0)
	{
		return;
	}

	for (auto i = 0; i < positions.size(); i++)
	{
		if (map->containsIdx(positions.at(i)))
		{
			sf::RectangleShape box;
			box.setFillColor({ 255,255,255,100 });
			box.setSize({ (float)map->getTileSize(), (float)map->getTileSize() });
			box.setOrigin(box.getLocalBounds().getCenter());
			box.setPosition(map->tileIdxToPos(positions.at(i)));

			win.draw(box);
		}
	}
}

std::vector<sf::Vector2<int>> EditorState::getBrushPositions(sf::Vector2<int> pos)
{
	std::vector<sf::Vector2<int>> res;

	sf::Vector2<int> offset = {pos.x - (int)(brushShape.front().size()/2), pos.y - (int)(brushShape.size() / 2)};

	for (auto i = 0; i < brushShape.size(); i++)
	{
		for (auto j = 0; j < brushShape.at(i).size(); j++)
		{
			if (brushShape.at(i).at(j) && map->containsIdx({ offset.x + j, offset.y + i }))
			{
				res.push_back({offset.x + j, offset.y + i});
			}
		}
	}

	return res;
}

void EditorState::setBrushCircle(float radius)
{
	//NOT CORRECTLY IMPLEMENTED
	brushShape.clear();

	int centerX = (int)std::round(radius);
	int centerY = (int)std::round(radius);
	int size = 2 * (int)std::round(radius);
	
	for (auto i = 0; i < size; i++)
	{
		brushShape.push_back(std::vector<bool>());
		for (auto j = 0; j < size; j++)
		{
			
			if (utl::gridDist(j,i,centerX,centerY) <= radius)
			{
				brushShape.at(i).push_back(true);
			}
			else
			{
				brushShape.at(i).push_back(false);
			}
		}
	}

}

void EditorState::setBrushSquare(int width, int height)
{

	brushShape.clear();

	for (auto i = 0; i < height; i++)
	{
		brushShape.push_back(std::vector<bool>());
		for (auto j = 0; j < width; j++)
		{
			brushShape.at(i).push_back(true);
		}
	}
}

int EditorState::createMap(unsigned int width, unsigned int height)
{
	delete map;
	this->clearUndoStack();
	this->clearRedoStack();

	map = new Map(texReg, 32, { width,height }, "tileset_0", 32);
	map->setVeiw(&mapView);
	map->setGridDraw(true);
	map->setGridColor(sf::Color::White);

	return 1;
}

int EditorState::saveMap()
{
	auto selection = pfd::save_file("Select a file", "", {}, false).result();
	if (!selection.empty())
	{
		if (!map->writeToFile(selection))
		{
			std::cout << "Could not save file..." << std::endl;
			return 0;
		}
	}
	else
	{
		return 0;
	}

	std::cout << "File Saved..." << std::endl;
	return 1;
}

int EditorState::openMap()
{

	auto selection = pfd::open_file("Select a file", "", {},false).result();
	if (!selection.empty())
	{
		Map* newMap = new Map(texReg);
		//check for succesful load
		if (newMap->loadFromFile(selection.front()))
		{
			//Then swap maps
			delete map;
			map = newMap;
			filepath = selection.front();
			this->clearRedoStack();
			this->clearUndoStack();
		}
		else
		{
			delete newMap;
		}
	}
		

	return 0;
}

void EditorState::exitEditor()
{
	this->isFinished = true;
}


void EditorState::undo()
{
	if (undoStack.empty()) { return; }

	undoStack.top()->undo();

	redoStack.push(undoStack.top());

	undoStack.pop();
}

void EditorState::redo()
{
	if (redoStack.empty()) { return; }

	redoStack.top()->execute();

	undoStack.push(redoStack.top());

	redoStack.pop();
}

void EditorState::clearUndoStack()
{
	while (!undoStack.empty())
	{
		delete undoStack.top();
		undoStack.pop();
	}
}

void EditorState::clearRedoStack()
{
	while (!redoStack.empty())
	{
		delete redoStack.top();
		redoStack.pop();
	}
}


void EditorState::showMenuBar_File()
{
	if (ImGui::MenuItem("New", "CTRL+N")) { doShowGuiNewMap = true; }
	if (ImGui::MenuItem("Save", "CTRL+S")) { std::cout << "Saving..." << std::endl; this->saveMap(); }
	if (ImGui::MenuItem("Open", "CTRL+O")) { std::cout << "Opening..." << std::endl; this->openMap(); }
	if (ImGui::MenuItem("Exit", "")) { std::cout << "Exiting..." << std::endl; this->exitEditor(); }
}

void EditorState::ShowMenuBar_Options()
{
	//if (ImGui::MenuItem("Show Grid", "CTRL+G")) {}
	ImGui::Text("Grid Options");
	ImGui::Separator();
	if (ImGui::Checkbox("Show Grid", &showMapGrid)) { map->setGridDraw(showMapGrid); }

	if (ImGui::BeginMenu("Colors"))
	{
		float sz = ImGui::GetTextLineHeight();
		for (int i = 0; i < gridColorOptions.size(); i++)
		{
			const char* name = gridColorNames.at(i).c_str();
			sf::Color col = gridColorOptions.at(i);
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), IM_COL32(col.r, col.g, col.b, col.a));
			ImGui::Dummy(ImVec2(sz, sz));
			ImGui::SameLine();
			if (ImGui::MenuItem(name))
			{
				map->setGridColor(col);
			}
		}
		ImGui::EndMenu();
	}

}

void EditorState::showMenuBar_Edit()
{
	if (ImGui::MenuItem("Cut", "CTRL+X")) {}
	if (ImGui::MenuItem("Copy", "CTRL+C")) {}
	if (ImGui::MenuItem("Paste", "CTRL+V")) {}
	ImGui::Separator();
	if (ImGui::MenuItem("Undo", "CTRL+Z", false, !undoStack.empty())) { this->undo();}
	if (ImGui::MenuItem("Redo", "CTRL+Y", false, !redoStack.empty())) { this->redo(); }
}

void EditorState::showMenuBar_Window()
{
	if (ImGui::MenuItem("Tile Pallette")) { doShowTileEditor = true; }
}

void EditorState::showGuiMenuBar()
{

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			showMenuBar_File();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Options"))
		{
			ShowMenuBar_Options();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			showMenuBar_Edit();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			showMenuBar_Window();
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void EditorState::showGuiNewMap(bool* pOpen)
{
	ImGuiWindowFlags windowFlags = 0;


	ImGui::SetWindowPos(ImVec2(0, 30));
	ImGui::SetWindowSize(ImVec2(300, 400.f));

	ImGui::Begin("newMap", pOpen, windowFlags);

	//Name
	ImGui::Text("Map Name:");
	ImGui::SameLine();
	ImGui::InputText("###nma", newMapName, IM_ARRAYSIZE(newMapName));

	//Size
	ImGui::Text("Width:");
	ImGui::SameLine();
	ImGui::InputInt("###nmb",&newMapWidth);
	ImGui::Text("Height:");
	ImGui::SameLine();
	ImGui::InputInt("###nmc", &newMapHeight);

	//Buttons
	if (ImGui::Button("Ok##nmd"))
	{
		this->createMap((unsigned int)newMapWidth, (unsigned int)newMapHeight);
		//Set map filename NOT IMPLEMENTED

		*pOpen = false;
		strncpy(newMapName, "", 512);
		newMapWidth = 10;
		newMapHeight = 10;
	}

	ImGui::SameLine();


	if (ImGui::Button("Cancel##nmd"))
	{
		*pOpen = false;
		strncpy(newMapName,"",512);
		newMapWidth = 10;
		newMapHeight = 10;
	}

	ImGui::End();
}

void EditorState::showGuiTileEditor(bool* pOpen)
{
	ImGuiWindowFlags windowFlags = 0;
	

	ImGui::SetWindowPos(ImVec2(0, 30));
	ImGui::SetWindowSize(ImVec2(300, 400.f));

	ImGui::Begin("palleteTool", pOpen, windowFlags);
	
	/*
	Components:
		- Dropdown to select from available tilesets
		- Buttons to add and remove tilesets
		- Control to change tilesets texture size

		-grid of controls to select index from active tilesets
		-controls to affect grid layout (really just zoom)
	*/

	//Tileset selector (Combo Box)
	ImGuiComboFlags flags = 0;
	std::vector<std::pair<std::string, int>> tilesets = map->getTilesets();

	const char* combo_preview_value = tilesets.at(tilsetSelIdx).first.c_str();
	if (ImGui::BeginCombo("tilsetSel", combo_preview_value, flags))
	{
		for (int i = 0; i < tilesets.size(); i++)
		{
			const bool is_selected = (tilsetSelIdx == i);
			if (ImGui::Selectable(tilesets.at(i).first.c_str(), is_selected))
			{
				tilsetSelIdx = i;
				brushFill.sheet = i;
				brushFill.type = -1;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
				
		}
		ImGui::EndCombo();
	}


	//Tileset Add and remove (buttons)
	if (ImGui::Button("Add"))
	{
		//Show Add Tilset Dialog
		ImVec2 pos = ImGui::GetWindowPos();
		pos.x += (.25f * ImGui::GetWindowWidth());
		pos.y += (.25f * ImGui::GetWindowHeight());
		tsAddPos = pos;
		doShowTilesetAdd = true;
	}

	ImGui::SameLine();

	if (ImGui::Button("Rem") && tilesets.size() > 1)
	{
		//Show Remove Tileset Dialog
	}

	//Tileset Texture size


	//Tilest Tile Select Grid
	float padding = 1.f;
	int cols = 5; //Set
	int rows = 1; //Calculated

	sf::Texture* tex = texReg->lookup(tilesets.at(tilsetSelIdx).first);
	unsigned int framesize = (unsigned int)tilesets.at(tilsetSelIdx).second;

	std::vector<sf::Sprite> tiles;

	

	//#tiles = (sheet size.x / texturesize) * (sheetsize.y / texturesize)
	int numTiles = (tex->getSize().x / framesize) * (tex->getSize().y / framesize);

	cols = (tex->getSize().x / framesize);

	if (numTiles < cols)
	{
		cols = numTiles;
	}

	//Rows = # tiles / cols
	rows = numTiles / cols;

	

	//Size of each tile = (window width - margins - tile padding) / cols
	float sz = (300.f - 20.f - padding) / (float)cols;

	Spritesheet sheet;
	sheet.setTexture(tex);
	sheet.setFrameSize(framesize, framesize);

	sf::Vector2f gridOffset = { 0,0 };

	ImGui::Separator();

	ImGui::BeginChild("tileGrid",ImVec2(300.f-40.f, 300.f-40.f));

	for (auto i = 0; i < rows; i++)
	{
		for (auto j = 0; j < cols; j++)
		{
			float posX = gridOffset.x + (j * (sz + 10.f));
			float posY = gridOffset.y + (i * (sz + 10.f));
			ImGui::SetCursorPos(ImVec2(posX,posY));
			int flatIdx = (i * cols) + j;
			/*ImVec2 p = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddImage(tex,);
			ImGui::Dummy(ImVec2(sz, sz));*/
			sheet.setFrame(flatIdx);
			sf::Sprite s(*tex);
			s.setTextureRect(sheet.getTextureRect());
			sheet.setScale({sz / s.getLocalBounds().size.x, sz / s.getLocalBounds().size.y});
			
			const bool is_selected = (tileSelIdx == flatIdx);
			ImGui::PushID(flatIdx);
			/*if (ImGui::Selectable("###", is_selected,0,ImVec2(sz,sz)))
			{
				tileSelIdx = flatIdx;
				brushFill.type = flatIdx;
			}*/


			if (ImGui::ImageButton("###", s, {sz,sz}))
			{
				tileSelIdx = flatIdx;
				brushFill.type = flatIdx;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}

			
			ImGui::PopID();
		}
	} //End grid

	//## Cursor Buttons
	sf::Sprite selIcon(*texReg->lookup("cursor_sel"));
	sf::Sprite circleIcon(*texReg->lookup("circleIcon"));
	sf::Sprite squareIcon(*texReg->lookup("squareIcon"));
	float size = ImGui::GetTextLineHeight();
	if (ImGui::ImageButton("###tez", selIcon, ImVec2(size,size)))
	{
		isPaintToolSelected = false;
	}

	ImGui::SameLine();

	if (ImGui::ImageButton("###tey", circleIcon, ImVec2(size, size)))
	{
		isPaintToolSelected = true;
		this->setBrushCircle(1.7);
	}

	ImGui::SameLine();

	if (ImGui::ImageButton("###tex", squareIcon, ImVec2(size, size)))
	{
		isPaintToolSelected = true;
		this->setBrushSquare(1,1);
	}


	ImGui::EndChild();

	ImGui::End();
}

void EditorState::showGuiTilesetAdd(bool* pOpen)
{
	ImGuiWindowFlags windowFlags = 0;


	//ImGui::SetWindowPos(tsAddPos);
	//ImGui::SetWindowSize(ImVec2(400, 100.f));

	

	ImGui::Begin("tilesetAdd", pOpen, windowFlags);


	//Image file select
	ImGui::Text("Texture File:");
	ImGui::SameLine();
	ImGui::InputText("###aa",tilesetTexturePath,IM_ARRAYSIZE(tilesetTexturePath));
	if (ImGui::Button("Browse"))
	{
		auto selection = pfd::open_file("Select a file", "", {".png"}, false).result();
		
		if (!selection.empty())
		{

			std::strncpy(tilesetTexturePath, selection.front().c_str(), 512);

			//Check if texture is already used under a different name
			std::string existingName = texReg->getKey(tilesetTexturePath);
			if (existingName != "")
			{
				//Then it exists, autofill the name
				std::strncpy(tilesetTextureName, existingName.c_str(), 512);
			}
		}
	}

	//Tileset Name
	ImGui::Text("Tileset Name:");
	ImGui::SameLine();
	ImGui::InputText("###za", tilesetTextureName, IM_ARRAYSIZE(tilesetTextureName));

	//Size select
	ImGui::Text("Texture Size: ");
	ImGui::SameLine();
	ImGui::InputInt("###ab", &tsAddTexSize);

	//Add and Cancel Button
	if (ImGui::Button("Add##ac"))
	{
		std::string tsetName = tilesetTextureName;
		std::string tsetTexPath = tilesetTexturePath;

		

		//Validate inputs
		if (tsetName == "")
		{
			std::strncpy(tilesetTexturePath, "", 512);
			std::strncpy(tilesetTextureName, "", 512);
			tsAddTexSize = 32;
			*pOpen = false;
			return;
		}

		//Check if key is in registry
		if (texReg->getPath(tilesetTexturePath) == "")
		{
			//Then check if the texture path is in the registy
			if (texReg->getKey(tilesetTexturePath) == "")
			{
				//Then add the new texture and key to the registry
				texReg->addTexture(tilesetTextureName, tilesetTexturePath);
			}
		}

		//Only add unique tilesets
		bool containsKey = false;
		for (auto s : map->getTilesets())
		{
			if (s.first == tilesetTextureName)
			{
				containsKey = true;
				break;
			}
		}

		if (!containsKey)
		{
			map->addTileset({ tsetName,tsAddTexSize });
		}

		
		std::strncpy(tilesetTexturePath, "", 512);
		std::strncpy(tilesetTextureName, "", 512);
		tsAddTexSize = 32;
		*pOpen = false;

	}

	ImGui::SameLine();

	if (ImGui::Button("Cancel##ad"))
	{
		std::strncpy(tilesetTexturePath, "", 512);
		std::strncpy(tilesetTextureName, "", 512);
		tsAddTexSize = 32;
		*pOpen = false;
	}

	ImGui::End();
}


EditorState::EditorState(TextureRegistry* textureRegistry, sf::RenderWindow* window)
	: Gamestate(textureRegistry, window)
{
	mapView = win->getDefaultView();
	this->init();
}

EditorState::~EditorState()
{

}

void EditorState::update(float dt)
{
	sf::Vector2<int> mousePos = sf::Mouse::getPosition(*win);
	win->setView(mapView);
	sf::Vector2<float> mapViewMousePos = win->mapPixelToCoords(mousePos);

	this->updateCamera(dt);

	if (doShowDemoGui) { ImGui::ShowDemoWindow(); }

	this->showGuiMenuBar();
	if(doShowTileEditor){ this->showGuiTileEditor(&doShowTileEditor); }
	if (doShowTilesetAdd) { this->showGuiTilesetAdd(&doShowTilesetAdd); }
	if (doShowGuiNewMap) { this->showGuiNewMap(&doShowGuiNewMap); }

	

	textBrushSheet.setString("Sheet: " + std::to_string(brushFill.sheet));
	textBrushType.setString("Type: " + std::to_string(brushFill.type));

	//Painting
	if (isPainting)
	{
		lastIdx = currIdx;
		currIdx = map->posToTileIdx(mapViewMousePos);
		if (currIdx != lastIdx && map->containsIdx(currIdx))
		{
			for (auto p : this->getBrushPositions(currIdx)) { 
				paintedTiles.push_back(p); 
				originalStates.push_back(Tile(*map->tileAtIdx((size_t)p.x, (size_t)p.y)));
			}
			ActionPaint singlePaintAction = ActionPaint(map, this->getBrushPositions(currIdx), brushFill);
			singlePaintAction.execute();
		}
	}

	//Tile highlight
	if (map->containsPos(mapViewMousePos))
	{
		isBrushDrawn = true;
	}
	else
	{
		isBrushDrawn = false;
	}
}

void EditorState::poll(sf::RenderWindow& win, std::optional<sf::Event> event)
{
	ImGuiIO& io = ImGui::GetIO();

	sf::Vector2<int> mousePos = sf::Mouse::getPosition(win);
	win.setView(mapView);
	sf::Vector2<float> mapViewMousePos = win.mapPixelToCoords(mousePos);

	//## Mouse press
	if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>())
	{
		sf::Vector2<int> mousePos = sf::Mouse::getPosition(win);
		win.setView(mapView);
		sf::Vector2<float> mapViewMousePos = win.mapPixelToCoords(mousePos);

		//Left Button
		if (mouseButton->button == sf::Mouse::Button::Left)
		{
			if (!io.WantCaptureMouse)
			{
				if (isPaintToolSelected && !isPainting && map->containsPos(mapViewMousePos))
				{
					isPainting = true; //Set painting flag
				}
			}
			
		}

		//Middle Button
		if (!io.WantCaptureMouse && mouseButton->button == sf::Mouse::Button::Middle)
		{
			isPan = true;
			panStart = {(float)mousePos.x, (float)mousePos.y};
		}

	}

	//## Mouse release
	if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonReleased>())
	{
		

		//Left
		if (mouseButton->button == sf::Mouse::Button::Left)
		{
			if (isPainting)
			{
				//If was painting and has now finished
				isPainting = false;
				ActionPaint* totalPaintAction = new ActionPaint(map,paintedTiles, brushFill);
				totalPaintAction->setOriginalState(originalStates);
				originalStates.clear();
				paintedTiles.clear();

				undoStack.push(totalPaintAction);
				this->clearRedoStack();
			}
		}

		//Middle Button
		if (mouseButton->button == sf::Mouse::Button::Middle)
		{
			//Dropper
			auto idx = map->posToTileIdx(mapViewMousePos);
			if (map->containsIdx(idx))
			{
				//std::cout << "Brush Sheet Before: " << brushFill.sheet << " | Brush Type Before: " << brushFill.type << std::endl;
				brushFill = Tile(*map->tileAtIdx((size_t)(idx.x), (size_t)(idx.y)));
				//std::cout << "Brush Sheet After: " << brushFill.sheet << " | Brush Type After: " << brushFill.type << std::endl;
			}

			//Pan
			if (isPan)
			{
				isPan = false;
			}
		}
	}


	//## Scroll Wheel
	if (const auto* mouseScrolled = event->getIf<sf::Event::MouseWheelScrolled>())
	{
		if (canZoom && !io.WantCaptureMouse)
		{
			float zoom = currZoom - (zoomSpeed * mouseScrolled->delta);
			zoomCamera(zoom);
		}

	}


	//## Key Press
	if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>())
	{
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z))
		{
			//Then undo
			this->undo();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Y))
		{
			//Then undo
			this->redo();
		}
	}

	//## Key Release
	if (const auto* keyRel = event->getIf<sf::Event::KeyReleased>())
	{
		if (keyRel->code == sf::Keyboard::Key::Grave)
		{
			doShowDemoGui = !doShowDemoGui;
		}
	}
}

void EditorState::draw(sf::RenderWindow& win)
{
	win.setView(mapView);
	map->Draw(win);
	if (isPaintToolSelected) { this->drawBrush(win); }


	win.setView(win.getDefaultView());
	win.draw(textBrushSheet);
	win.draw(textBrushType);
}


