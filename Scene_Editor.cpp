#include "Scene_Editor.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <filesystem>

Scene_Editor::Scene_Editor(sf::RenderWindow* pWindow, GameEngine* pGame)
	: Scene(pWindow, pGame)
{
	Initialize();
}

void Scene_Editor::Initialize()
{
	m_currentView = eEditorView::Main; 
	CreateBackground();
	RegisterSceneActions();
	CreateMenus();
}

void Scene_Editor::CreateBackground()
{
	EntityPointer pBg0 = m_entityMgr.addEntity(eEntityTag::Background);
	EntityPointer pBg1 = m_entityMgr.addEntity(eEntityTag::Background);

	pBg0->AddComponent<CAnimation>(m_pGame->GetAssets().GetAnimation(eAsset::SpaceBg));
	pBg1->AddComponent<CAnimation>(m_pGame->GetAssets().GetAnimation(eAsset::SpaceBg));

	pBg0->AddComponent<CTransform>();
	pBg1->AddComponent<CTransform>();

	Vec2 viewCenter = Vec2(m_pWindow->getView().getCenter().x, m_pWindow->getView().getCenter().y);
	pBg0->GetComponent<CTransform>().pos = viewCenter;
	pBg1->GetComponent<CTransform>().pos = Vec2(viewCenter.x, viewCenter.y - pBg1->GetComponent<CAnimation>().animation.GetSize().y);
}

void Scene_Editor::RegisterSceneActions()
{
	RegisterAction(sf::Keyboard::Escape, eAction::Quit);
	RegisterAction(sf::Keyboard::P, eAction::Pause);
	RegisterAction(sf::Keyboard::T, eAction::ToggleTexture);
	RegisterAction(sf::Keyboard::C, eAction::ToggleCollision);
	RegisterAction(sf::Keyboard::Tab, eAction::ToggleHotMenu);
	RegisterAction(sf::Keyboard::Enter, eAction::TogglePauseMenu);
	RegisterAction(sf::Keyboard::Enter, eAction::SelectOption);
	RegisterAction(sf::Keyboard::G, eAction::ToggleGrid);
	RegisterAction(sf::Keyboard::W, eAction::MoveUp);
	RegisterAction(sf::Keyboard::S, eAction::MoveDown);
	RegisterAction(sf::Keyboard::A, eAction::CycleLeft);
	RegisterAction(sf::Keyboard::D, eAction::CycleRight);
	RegisterAction(sf::Keyboard::P, eAction::Save);
	RegisterAction(sf::Keyboard::L, eAction::Load);
	RegisterAction(sf::Keyboard::N, eAction::New);
}

void Scene_Editor::Update()
{
	m_entityMgr.Update();
	sCamera();
	sBackgroundScroll();
	sParallax();
	sDrag();
	sRender();
	m_currentFrame++;
}

void Scene_Editor::sDrag()
{
	EntityList eList = *(m_entityMgr.getEntities());
	sf::Vector2f mousePos = m_pWindow->mapPixelToCoords(sf::Mouse::getPosition(*m_pWindow));

	for (EntityPointer e : eList)
	{
		if (e->HasComponent<CDraggable>() && e->GetComponent<CDraggable>().dragging)
		{
			e->GetComponent<CTransform>().pos = Vec2(mousePos.x, mousePos.y);
		}
	}
}

void Scene_Editor::sCamera()
{
	sf::Vector2f cameraSize = sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT);
	sf::View oldView = m_pWindow->getView();
	sf::Vector2f newOrigin = sf::Vector2f(oldView.getCenter().x, oldView.getCenter().y - m_screenScrollSpeed);
	sf::View view(newOrigin, cameraSize);
	m_pGame->UpdateView(view);
}

void Scene_Editor::SpawnEnemy(eAsset asset, eBehavior behave, Vec2 pos)
{
	auto pEnemy = m_entityMgr.addEntity(eEntityTag::Enemy);

	pEnemy->AddComponent<CAnimation>(m_pGame->GetAssets().GetAnimation(asset));

	pEnemy->AddComponent<CTransform>();
	pEnemy->GetComponent<CTransform>().pos = pos;

	sf::RectangleShape boundBox = sf::RectangleShape(pEnemy->GetComponent<CAnimation>().animation.GetSize().ToSFMLVec2());
	boundBox.setSize(sf::Vector2f(boundBox.getSize().x, boundBox.getSize().y));
	boundBox.setOrigin(boundBox.getSize().x / 2, boundBox.getSize().y / 2);
	boundBox.setFillColor(sf::Color::Red);
	pEnemy->AddComponent<CBoundingBox>(boundBox);

	pEnemy->AddComponent<CCollision>();

	pEnemy->AddComponent<CHealth>(5);

	std::vector<Vec2> posList;
	switch(behave)
	{
	case eBehavior::Patrol:
		posList = { Vec2(0,pos.y), Vec2(WINDOW_WIDTH, pos.y) };
		pEnemy->AddComponent<CBehavior>(eBehavior::Patrol, posList);
		break;
	default:
		break;
	}

	pEnemy->AddComponent<CDraggable>();
}

void Scene_Editor::sRender()
{
	m_pWindow->clear(sf::Color::Black);

	RenderBackground();

	EntityList allEntities = *(m_entityMgr.getEntities());
	for (EntityPointer e : allEntities)
	{
		if (e->getTag() == eEntityTag::Background) { continue; }

		if (m_drawTextures && e->HasComponent<CAnimation>())
		{
			e->GetComponent<CAnimation>().animation.Update();
			if (e->GetComponent<CAnimation>().animation.IsEnded())
			{
				e->markInactive();
			}
			CTransform transform = e->GetComponent<CTransform>();
			e->GetComponent<CAnimation>().animation.GetSprite().setPosition(transform.pos.x, transform.pos.y);
			e->GetComponent<CAnimation>().animation.GetSprite().setRotation(transform.angle);
			e->GetComponent<CAnimation>().animation.GetSprite().setScale(transform.scale.x, transform.scale.y);
			if (e->GetComponent<CInvincibility>().has && e->GetComponent<CInvincibility>().duration > 0)
			{
				e->GetComponent<CAnimation>().animation.GetSprite().setColor(sf::Color(255, 255, 255, 100));
			}
			else if (e->GetComponent<CInvincibility>().has && e->GetComponent<CInvincibility>().duration <= 0)
			{
				e->GetComponent<CAnimation>().animation.GetSprite().setColor(sf::Color(255, 255, 255, 255));
			}
			m_pWindow->draw(e->GetComponent<CAnimation>().animation.GetSprite());
		}
	}

	DrawHud();
	RenderLoadMenu();
	m_pWindow->display();
}

void Scene_Editor::DrawHud()
{
	std::string controls;
	std::string info;
	switch (m_currentView)
	{
	case eEditorView::Main:
		controls =
"N: new enemy \n\
B: cycle background \n\
P: save current to file \n\
L: load level file \n\
G: play current level \n\
W/S: move camera \n";
		info = "Main View\n" + m_currentFile;
		break;
	case eEditorView::New:
		RenderAssetPreview();
		controls =
"N: exit view \n\
L-Click: Spawn enemy \n\
Scroll or A/D: Cycle enemies \n\
W/S: move camera \n";
		info = "New View";
		break;
	default:
		break;
	}

	// Create a text object
	sf::Text textControls;
	textControls.setFont(m_pGame->GetAssets().GetFont(eAsset::SerifFont)); // Set the font
	textControls.setString(controls); // Set the string to display
	textControls.setCharacterSize(20); // Set the character size in pixels
	textControls.setFillColor(sf::Color::Red); // Set the fill color
	textControls.setPosition(20, 50); // Set the position of the text
	textControls.setOutlineColor(sf::Color::Black);
	textControls.setOutlineThickness(2);

	sf::Text textInfo;
	textInfo.setFont(m_pGame->GetAssets().GetFont(eAsset::SerifFont)); // Set the font
	textInfo.setString(info); // Set the string to display
	textInfo.setCharacterSize(20); // Set the character size in pixels
	textInfo.setFillColor(sf::Color::Red); // Set the fill color
	textInfo.setPosition(WINDOW_WIDTH/2, 25); // Set the position of the text
	textInfo.setOutlineColor(sf::Color::Black);
	textInfo.setOutlineThickness(2);

	sf::FloatRect textRect = textInfo.getLocalBounds();
	textInfo.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

	sf::View worldView = m_pWindow->getView();
	sf::View hudView(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
	m_pWindow->setView(hudView);
	m_pWindow->draw(textControls);
	m_pWindow->draw(textInfo);
	m_pWindow->setView(worldView);
}

void Scene_Editor::RenderAssetPreview()
{
	sf::Vector2f mousePos = m_pWindow->mapPixelToCoords(sf::Mouse::getPosition(*m_pWindow));

	sf::Sprite preview = m_pGame->GetAssets().GetAnimation(m_editorAssets[m_editorAssetsIdx]).GetSprite();
	preview.setPosition(sf::Vector2f(mousePos.x, mousePos.y));
	preview.setColor(sf::Color(255, 255, 255, 100));
	m_pWindow->draw(preview);
}

void Scene_Editor::RenderBackground()
{
	EntityList backgrounds = *(m_entityMgr.getEntities(eEntityTag::Background));
	for (EntityPointer e : backgrounds)
	{
		if (e->HasComponent<CAnimation>())
		{
			e->GetComponent<CAnimation>().animation.Update();
			if (e->GetComponent<CAnimation>().animation.IsEnded())
			{
				e->markInactive();
			}
			CTransform transform = e->GetComponent<CTransform>();
			e->GetComponent<CAnimation>().animation.GetSprite().setPosition(transform.pos.x, transform.pos.y);
			e->GetComponent<CAnimation>().animation.GetSprite().setRotation(transform.angle);

			m_pWindow->draw(e->GetComponent<CAnimation>().animation.GetSprite());
		}
	}
}

void Scene_Editor::sDoAction(Action action)
{
	switch (m_currentView)
	{
	case eEditorView::Main:
		MainDoAction(action);
		break;
	case eEditorView::New:
		NewDoAction(action);
		break;
	case eEditorView::Load:
		LoadDoAction(action);
		break;
	}
}

void Scene_Editor::NewDoAction(Action action)
{
	if (action.GetType() == eActionType::Start)
	{
		switch (action.GetName())
		{
		case eAction::Pause:
			m_isPaused = !m_isPaused;
			break;
		case eAction::Quit:
			m_isEnded = true;
			break;
		case eAction::TogglePauseMenu:
			m_isPauseMenuActive = !m_isPauseMenuActive;
			break;
		case eAction::MoveUp:
			m_screenScrollSpeed = 10;
			break;
		case eAction::MoveDown:
			m_screenScrollSpeed = -10;
			break;
		case eAction::New:
			m_currentView = eEditorView::Main;
			break;
		case eAction::LClick:
			AddEntity();
			break;
		case eAction::WheelUp:
		case eAction::CycleRight:
			m_editorAssetsIdx = (m_editorAssetsIdx - 1 + m_editorAssets.size()) % m_editorAssets.size();
			break;
		case eAction::WheelDown:
		case eAction::CycleLeft:
			m_editorAssetsIdx = (m_editorAssetsIdx + 1) % m_editorAssets.size();
			break;
		case eAction::ActionCount:
			break;
		}
	}
	if (action.GetType() == eActionType::Stop)
	{
		switch (action.GetName())
		{
		case eAction::MoveUp:
		case eAction::MoveDown:
			m_screenScrollSpeed = 0;
			break;
		}
	}
}

void Scene_Editor::LoadDoAction(Action action)
{
	if (action.GetType() == eActionType::Start)
	{
		switch (action.GetName())
		{
		case eAction::Pause:
			m_isPaused = !m_isPaused;
			break;
		case eAction::Quit:
			m_currentView = eEditorView::Main;
			m_isLoadMenuActive = false;
			break;
		case eAction::SelectOption:
			LoadLevelFromFile("levels/"+m_loadMenuItems[m_selectedLoadMenuIndex].getString());
			break;
		case eAction::MoveUp:
			m_selectedLoadMenuIndex = (m_selectedLoadMenuIndex - 1 + m_loadMenuItems.size()) % m_loadMenuItems.size();
			break;
		case eAction::MoveDown:
			m_selectedLoadMenuIndex = (m_selectedLoadMenuIndex + 1) % m_loadMenuItems.size();
			break;
		case eAction::ActionCount:
			break;
		}
	}
}

void Scene_Editor::AddEntity()
{
	sf::Vector2f mousePos = m_pWindow->mapPixelToCoords(sf::Mouse::getPosition(*m_pWindow));
	eAsset asset = m_editorAssets[m_editorAssetsIdx];
	SpawnEnemy(asset, eBehavior::Idle, Vec2(mousePos.x, mousePos.y));
}

void Scene_Editor::SelectPauseMenuOption()
{
	if (m_pauseMenuItems[m_selectedPauseIndex].getString() == "Resume")
	{
		m_isPauseMenuActive = !m_isPauseMenuActive;
	}
	else if (m_pauseMenuItems[m_selectedPauseIndex].getString() == "Exit")
	{
		m_pGame->ReturnToMainMenu();
	}
}

void Scene_Editor::MainDoAction(Action action)
{
	if (action.GetType() == eActionType::Start)
	{
		switch (action.GetName())
		{
		case eAction::Pause:
			m_isPaused = !m_isPaused;
			break;
		case eAction::Quit:
			m_isEnded = true;
			break;
		case eAction::ToggleTexture:
			break;
		case eAction::ToggleCollision:
			break;
		case eAction::ToggleHotMenu:
			m_isHotMenuActive = !m_isHotMenuActive;
			break;
		case eAction::TogglePauseMenu:
			m_isPauseMenuActive = !m_isPauseMenuActive;
			break;
		case eAction::ToggleGrid:
			break;
		case eAction::ToggleCamera:
			break;
		case eAction::MoveUp:
			m_screenScrollSpeed = 10;
			break;
		case eAction::MoveDown:
			m_screenScrollSpeed = -10;
			break;
		case eAction::MoveLeft:
			break;
		case eAction::MoveRight:
			break;
		case eAction::New:
			m_currentView = eEditorView::New;
			break;
		case eAction::LClick:
			GrabEntity();
			break;
		case eAction::Save:
			SaveLevelToFile();
			break;
		case eAction::Load:
			m_currentView = eEditorView::Load;
			m_isLoadMenuActive = true;
			PopulateLoadMenu();
		case eAction::ActionCount:
			break;
		}
	}

	if (action.GetType() == eActionType::Stop)
	{
		switch (action.GetName())
		{
		case eAction::MoveUp:
		case eAction::MoveDown:
			m_screenScrollSpeed = 0;
			break;
		case eAction::LClick:
			ReleaseEntity();
			break;
		}
	}

}

void Scene_Editor::GrabEntity()
{
	EntityList eList = *(m_entityMgr.getEntities());
	sf::Vector2f mousePos = m_pWindow->mapPixelToCoords(sf::Mouse::getPosition(*m_pWindow));

	for (EntityPointer e : eList)
	{
		if (e->HasComponent<CDraggable>())
		{
			if (e->GetComponent<CAnimation>().animation.GetSprite().getGlobalBounds().contains(mousePos.x, mousePos.y))
			{
				e->GetComponent<CDraggable>().dragging = true;
			}
		}
	}
}

void Scene_Editor::ReleaseEntity()
{
	EntityList eList = *(m_entityMgr.getEntities());

	for (EntityPointer e : eList)
	{
		if (e->HasComponent<CDraggable>())
		{
			e->GetComponent<CDraggable>().dragging = false;
		}
	}
}

void Scene_Editor::sParallax()
{
	EntityList decs = *(m_entityMgr.getEntities(eEntityTag::Decoration));
	for (EntityPointer e : decs)
	{
		if (e->HasComponent<CParallax>())
		{
			float parallaxMultiplier = e->GetComponent<CParallax>().factor;
			e->GetComponent<CTransform>().pos.y -= SCREEN_SCROLL_SPEED * parallaxMultiplier;
		}
	}
}

float Scene_Editor::ConvertTransformRotationToRads(float rotation)
{
	return (-1 * rotation + 90) * acos(-1) / 180; //acos(-1)=pi
}

void Scene_Editor::sBackgroundScroll()
{
	EntityList bgs = *(m_entityMgr.getEntities(eEntityTag::Background));
	for (EntityPointer bg : bgs)
	{
		if (IsEntityOffScreen(bg))
		{
			bg->GetComponent<CTransform>().pos.y -= 2.0f * bg->GetComponent<CAnimation>().animation.GetSize().y;
		}
	}
}

bool Scene_Editor::IsEntityOffScreen(EntityPointer e)
{
	float eHeight = e->GetComponent<CAnimation>().animation.GetSize().y;
	float eCenter = e->GetComponent<CTransform>().pos.y;
	float topOfE = eCenter - eHeight / 2.0f;
	float cameraHeight = m_pWindow->getView().getSize().y;
	float cameraCenter = m_pWindow->getView().getCenter().y;
	float bottomOfView = cameraCenter + cameraHeight / 2.0f;

	return topOfE > bottomOfView;
}


void Scene_Editor::RenderItemMenu()
{
	sf::View worldView = m_pWindow->getView();
	sf::View menuView(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
	m_pWindow->setView(menuView);
	sf::Sprite bg;
	if (m_isPauseMenuActive || m_isHotMenuActive)
	{
		bg = m_pGame->GetAssets().GetAnimation(eAsset::MenuBack).GetSprite();
		bg.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
		m_pWindow->draw(bg);
	}

	if (m_isPauseMenuActive)
	{
		for (size_t i = 0; i < m_pauseMenuItems.size(); i++)
		{
			if (i == m_selectedPauseIndex)
			{
				m_pauseMenuItems[i].setFillColor(sf::Color::Red); // Highlight selected item
			}
			else
			{
				m_pauseMenuItems[i].setFillColor(sf::Color::White);
			}
			m_pWindow->draw(m_pauseMenuItems[i]);
		}
	}

	if (m_isHotMenuActive)
	{
		for (size_t i = 0; i < m_hotMenuItems.size(); i++)
		{
			if (i == m_selectedHotMenuIndex)
			{
				m_hotMenuItems[i].setFillColor(sf::Color::Red); // Highlight selected item
			}
			else
			{
				m_hotMenuItems[i].setFillColor(sf::Color::White);
			}
			m_pWindow->draw(m_hotMenuItems[i]);
		}
	}
	m_pWindow->setView(worldView);
}

void Scene_Editor::CreateMenus()
{
	sf::Font font = m_pGame->GetAssets().GetFont(eAsset::SerifFont);
	std::vector<std::string> optionStrings = { "Resume", "Options", "Exit" };

	for (size_t i = 0; i < optionStrings.size(); ++i)
	{
		sf::Text optionText(optionStrings[i], m_pGame->GetAssets().GetFont(eAsset::SerifFont), 24);
		optionText.setFillColor(sf::Color::White);
		optionText.setPosition(300.f, 200.f + i * 50.f);
		m_pauseMenuItems.push_back(optionText);
	}

	std::vector<std::string> hotMenuOptionStrings;

	for (const auto& pair : specialHudMap)
	{
		hotMenuOptionStrings.push_back(pair.second);
	}

	for (size_t i = 0; i < hotMenuOptionStrings.size(); ++i)
	{
		sf::Text optionText(hotMenuOptionStrings[i], m_pGame->GetAssets().GetFont(eAsset::SerifFont), 24);
		optionText.setFillColor(sf::Color::White);
		optionText.setPosition(300.f, 200.f + i * 50.f);
		m_hotMenuItems.push_back(optionText);
	}
}

void Scene_Editor::SaveLevelToFile()
{
	// File path and name
	std::string filename = m_currentFile;
	if (filename == "new")
	{
		filename = "levels/custom-level-" + std::to_string(std::time(nullptr)) + ".txt";
		m_currentFile = filename;
	}

	// Create an ofstream object called 'file'
	std::ofstream file(filename);

	// Check if the file was opened successfully
	if (!file.is_open())
	{
		std::cout << "Error: Could not open the file '" << filename << "' for writing." << std::endl;
		exit(1); // Return with error code 1
	}

	EntityList eList = *(m_entityMgr.getEntities(eEntityTag::Enemy));
	for (EntityPointer e : eList)
	{
		file << (int)(e->GetComponent<CAnimation>().animation.GetName()) << " "
			<< (int)(e->GetComponent<CTransform>().pos.x) << " "
			<< (int)(e->GetComponent<CTransform>().pos.y) << " "
			<<  std::endl;
	}

	// Close the file
	file.close();

	std::cout << "File '" << filename << "' written successfully." << std::endl;
}

void Scene_Editor::LoadLevelFromFile(std::string filename)
{
	DestroyAllEntities(); //first clean up current level

	// Open the file
	std::ifstream file(filename);

	// Check if file opened successfully
	if (!file)
	{
		std::cerr << "Unable to open file: " << filename << std::endl;
		return;
	}

	std::string line;

	// Read file line by line
	while (getline(file, line))
	{
		eAsset asset = eAsset(stoi(GetAndDeleteNextStringToken(line)));
		int xPos = stoi(GetAndDeleteNextStringToken(line));
		int yPos = stoi(GetAndDeleteNextStringToken(line));

		SpawnEnemy(asset, eBehavior::Idle, Vec2(xPos, yPos));
	}

	m_currentFile = filename;
}

std::string Scene_Editor::GetAndDeleteNextStringToken(std::string& str)
{
	size_t whitespaceStartPos;
	std::string token;
	if (str.length() == 0) { return str; }
	if ((whitespaceStartPos = str.find(' ')) != std::string::npos)
	{
		size_t whitespaceEndPos = whitespaceStartPos;
		while (str[whitespaceEndPos + 1] == ' ')
		{
			whitespaceEndPos++;
		}
		token = str.substr(0, whitespaceStartPos);
		str.erase(0, whitespaceEndPos + 1);
	}
	else
	{
		token = str;
		str.erase(0, str.length());
	}
	return token;
}

void Scene_Editor::RenderLoadMenu()
{
	sf::View worldView = m_pWindow->getView();
	sf::View menuView(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
	m_pWindow->setView(menuView);
	sf::Sprite bg;

	if (m_isLoadMenuActive)
	{
		bg = m_pGame->GetAssets().GetAnimation(eAsset::MenuBack).GetSprite();
		bg.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
		//m_pWindow->draw(bg);

		for (size_t i = 0; i < m_loadMenuItems.size(); i++)
		{
			if (i == m_selectedLoadMenuIndex)
			{
				m_loadMenuItems[i].setFillColor(sf::Color::Red); // Highlight selected item
			}
			else
			{
				m_loadMenuItems[i].setFillColor(sf::Color::White);
			}
			m_pWindow->draw(m_loadMenuItems[i]);
		}
	}
}

void Scene_Editor::PopulateLoadMenu()
{
	std::vector<std::string> files = getFilenamesInDirectory("levels");

	for (size_t i = 0; i < files.size(); ++i)
	{
		sf::Text optionText(files[i], m_pGame->GetAssets().GetFont(eAsset::SerifFont), 24);
		optionText.setFillColor(sf::Color::White);
		optionText.setPosition(300.f, 200.f + i * 50.f);
		m_loadMenuItems.push_back(optionText);
	}
}

std::vector<std::string> Scene_Editor::getFilenamesInDirectory(const std::string& directoryPath)
{
	std::vector<std::string> filenames;

	// Check if the directory exists
	if (!std::filesystem::exists(directoryPath))
	{
		std::cerr << "Directory does not exist: " << directoryPath << std::endl;
		return filenames;
	}

	// Iterate over each entry in the directory
	for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
	{
		if (std::filesystem::is_regular_file(entry))
		{
			filenames.push_back(entry.path().filename().string());
		}
	}

	return filenames;
}

void Scene_Editor::DestroyAllEntities()
{
	EntityList eList = *(m_entityMgr.getEntities(eEntityTag::Enemy));

	for(EntityPointer e : eList)
	{
		e->markInactive();
	}
}