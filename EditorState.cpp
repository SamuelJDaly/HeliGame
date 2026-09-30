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
		return -1;
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
	//Pan
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
}

void EditorState::drawBrush(sf::RenderWindow& win)
{
	sf::Vector2<int> mousePos = sf::Mouse::getPosition(win);
	win.setView(mapView);
	sf::Vector2<float> mapViewMousePos = win.mapPixelToCoords(mousePos);
	sf::Vector2<int> mapIdx = map->posToTileIdx(mapViewMousePos);

	if (mapIdx.x < 0 || mapIdx.y < 0)
	{
		return;
	}

	sf::RectangleShape box;
	box.setFillColor({255,255,255,100});
	box.setSize({ (float)map->getTileSize(), (float)map->getTileSize()});
	box.setOrigin(box.getLocalBounds().getCenter());
	box.setPosition(map->tileIdxToPos(mapIdx));

	win.draw(box);
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
	if (ImGui::MenuItem("New", "CTRL+N")) { std::cout << "Creating New File..." << std::endl; }
	if (ImGui::MenuItem("Save", "CTRL+S")) { std::cout << "Saving..." << std::endl; }
	if (ImGui::MenuItem("Load", "CTRL+O")) { std::cout << "Loading..." << std::endl; }
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
	if (ImGui::MenuItem("Tile Pallette")) { doShowTilePallete = true; }
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

void EditorState::showGuiPalleteTool()
{
	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoResize;
	

	ImGui::SetWindowPos(ImVec2(0, 30));
	ImGui::SetWindowSize(ImVec2(300, 150));

	ImGui::Begin("palleteTool", &doShowDemoGui, windowFlags);
	


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
	if(doShowTilePallete){ this->showGuiPalleteTool(); }
	

	//Painting
	if (isPainting)
	{
		lastIdx = currIdx;
		currIdx = map->posToTileIdx(mapViewMousePos);
		if (currIdx != lastIdx && map->containsIdx(currIdx))
		{
			paintedTiles.push_back(currIdx);
			originalStates.push_back(Tile(*map->tileAtIdx((size_t)currIdx.x, (size_t)currIdx.y)));
			ActionPaint singlePaintAction = ActionPaint(map, { currIdx }, brushFill);
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
	//## Mouse press
	if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>())
	{
		sf::Vector2<int> mousePos = sf::Mouse::getPosition(win);
		win.setView(mapView);
		sf::Vector2<float> mapViewMousePos = win.mapPixelToCoords(mousePos);

		//Left Button
		if (mouseButton->button == sf::Mouse::Button::Left)
		{
			if (isPaintToolSelected && !isPainting && map->containsPos(mapViewMousePos))
			{
				isPainting = true; //Set painting flag
				//undoStates.push(currMap); //push current state to undo stack
			}
		}

	}

	//## Mouse release
	if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonReleased>())
	{
		sf::Vector2<int> mousePos = sf::Mouse::getPosition(win);
		win.setView(mapView);
		sf::Vector2<float> mapViewMousePos = win.mapPixelToCoords(mousePos);

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

		if (mouseButton->button == sf::Mouse::Button::Middle)
		{
			//Dropper
			auto idx = map->posToTileIdx(mapViewMousePos);
			if (map->containsIdx(idx))
			{
				std::cout << "Brush Sheet Before: " << brushFill.sheet << " | Brush Type Before: " << brushFill.type << std::endl;
				brushFill = Tile(*map->tileAtIdx((size_t)(idx.x), (size_t)(idx.y)));
				std::cout << "Brush Sheet After: " << brushFill.sheet << " | Brush Type After: " << brushFill.type << std::endl;
			}
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
	if (isBrushDrawn) { this->drawBrush(win); }

}


