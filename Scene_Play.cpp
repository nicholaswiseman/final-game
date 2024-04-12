#include "Scene_Play.h"
#include "Physics.h"
#include "Assets.h"
#include "Components.h"
#include "Action.h"
#include "eEntityTags.h"
#include "ePlayerState.h"

#include <fstream>
#include <iostream>

Scene_Play::Scene_Play(const std::string& levelPath, sf::RenderWindow* pWindow, GameEngine* pGame)
	: Scene(pWindow, pGame)
	, m_levelPath(levelPath)
{
	Init(m_levelPath);
}

void Scene_Play::Init(const std::string& levelPath)
{
	RegisterAction(sf::Keyboard::Escape, eAction::Quit);
	RegisterAction(sf::Keyboard::P, eAction::Pause);
	RegisterAction(sf::Keyboard::T, eAction::ToggleTexture);
	RegisterAction(sf::Keyboard::C, eAction::ToggleCollision);
	RegisterAction(sf::Keyboard::G, eAction::ToggleGrid);
	RegisterAction(sf::Keyboard::L, eAction::ToggleCamera);
	RegisterAction(sf::Keyboard::W, eAction::MoveUp);
	RegisterAction(sf::Keyboard::S, eAction::MoveDown);
	RegisterAction(sf::Keyboard::A, eAction::MoveLeft);
	RegisterAction(sf::Keyboard::D, eAction::MoveRight);
	RegisterAction(sf::Keyboard::Space, eAction::Attack);

	LoadLevel(levelPath);



}

void Scene_Play::LoadLevel(const std::string& levelPath)
{
	std::vector<std::string> level = ReadLevelFile(levelPath);

	for (std::string object : level)
	{
		SpawnObject(object);
	}

	SpawnMeat();
	SpawnPlayer();
}

void Scene_Play::SpawnObject(std::string obj)
{
	std::string objType = GetAndDeleteNextStringToken(obj);
	if (objType.compare("Tile") == 0)
	{
		SpawnTile(obj);
	}
	else if (objType.compare("Enemy") == 0)
	{
		SpawnEnemy(obj);
	}
	else if (objType.compare("Item") == 0)
	{
	}
}

void Scene_Play::SpawnTile(std::string obj)
{
	eAsset tileType = eAsset::AssetCount;
	std::string tileTypeStr = GetAndDeleteNextStringToken(obj);
	auto stringToTileMapEntry = m_charToTileMap.find(tileTypeStr);
	if (stringToTileMapEntry != m_charToTileMap.end())
	{
		tileType = stringToTileMapEntry->second;
	}

	int roomX = stoi(GetAndDeleteNextStringToken(obj));
	int roomY = stoi(GetAndDeleteNextStringToken(obj));
	int tileX = stoi(GetAndDeleteNextStringToken(obj));
	std::string tileystring = GetAndDeleteNextStringToken(obj);
	int tileY = stoi(tileystring);

	bool blocksMovement = stoi(GetAndDeleteNextStringToken(obj));
	bool blocksVision = stoi(GetAndDeleteNextStringToken(obj));

	auto tile = m_entityMgr.addEntity(eEntityTag::Tile);
	tile->AddComponent<CAnimation>(m_pGame->GetAssets().GetAnimation(tileType));
	tile->AddComponent<CTransform>();
	tile->GetComponent<CTransform>().scale = Vec2(4, 4);
	tile->GetComponent<CTransform>().pos = GridToMidPixel(roomX, roomY, tileX, tileY, tile);
	tile->GetComponent<CTransform>().prevPos = tile->GetComponent<CTransform>().pos;
	sf::RectangleShape boundBox = sf::RectangleShape(tile->GetComponent<CAnimation>().animation.GetSize().ToSFMLVec2());
	boundBox.setSize(sf::Vector2f(boundBox.getSize().x * 4, boundBox.getSize().y * 4));
	boundBox.setOrigin(boundBox.getSize().x / 2, boundBox.getSize().y / 2);
	boundBox.setFillColor(sf::Color::Red);
	tile->AddComponent<CBoundingBox>(boundBox);
	if (blocksMovement)
	{
		tile->AddComponent<CCollision>();
	}
	if (blocksVision)
	{
		tile->AddComponent<CBlockVision>();
		tile->GetComponent<CBlockVision>().blocksVision = true;
	}


}

void Scene_Play::SpawnSword()
{
	EntityPointer pPlayer = m_entityMgr.getPlayer();
	EntityPointer pSword = m_entityMgr.addEntity(eEntityTag::Attack);
	pSword->AddComponent<CAnimation>(m_pGame->GetAssets().GetAnimation(eAsset::Sword));
	sf::RectangleShape boundBox = sf::RectangleShape(pSword->GetComponent<CAnimation>().animation.GetSize().ToSFMLVec2());
	boundBox.setSize(sf::Vector2f(boundBox.getSize().x * 4, boundBox.getSize().y * 4));
	boundBox.setOrigin(boundBox.getSize().x / 2, boundBox.getSize().y / 2);
	boundBox.setFillColor(sf::Color::Red);
	pSword->AddComponent<CBoundingBox>(boundBox);
	pSword->AddComponent<CTransform>();
	UpdateSwordTransform(pSword);
	pSword->AddComponent<CCollision>();
	pSword->AddComponent<CDamage>(1);
	pPlayer->GetComponent<CInput>().canAttack = false;
}

Vec2 Scene_Play::GridToMidPixel(float roomX, float roomY, float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	float roomOffsetX = roomX * WINDOW_WIDTH;
	float roomOffsetY = roomY * WINDOW_HEIGHT;
	float entityHalfWidth = (entity->GetComponent<CAnimation>().animation.GetSize().x * entity->GetComponent<CTransform>().scale.x) / 2;
	float entityHalfHeight = (entity->GetComponent<CAnimation>().animation.GetSize().y * entity->GetComponent<CTransform>().scale.x) / 2;
	float xPixel = (gridX * GRID_SIZE_PIXELS) + entityHalfWidth + roomOffsetX;
	float yPixel = (gridY * GRID_SIZE_PIXELS) + entityHalfHeight + roomOffsetY;

	return Vec2(xPixel, yPixel);
}

std::string Scene_Play::GetAndDeleteNextStringToken(std::string& str)
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

std::vector<std::string> Scene_Play::ReadLevelFile(std::string levelPath)
{
	std::ifstream levelFile(levelPath);
	std::vector<std::string> levelTxt;

	if (levelFile.is_open())
	{
		std::string buffer;
		while (getline(levelFile, buffer))
		{
			levelTxt.insert(levelTxt.begin(), buffer);
		}
	}
	else
	{
		std::cout << "!!ERROR - Failed to read level file" << levelPath << std::endl;
		exit(-1);
	}
	levelFile.close();

	return levelTxt;
}

void Scene_Play::SpawnPlayer()
{
	auto player = m_entityMgr.addEntity(eEntityTag::Player);

	player->AddComponent<CAnimation>(m_pGame->GetAssets().GetAnimation(eAsset::LinkIdleDown));

	player->AddComponent<CTransform>();
	player->GetComponent<CTransform>().scale = Vec2(4, 4);
	player->GetComponent<CTransform>().pos = GridToMidPixel(0, 0, 8, 5, player);

	sf::RectangleShape boundBox = sf::RectangleShape(player->GetComponent<CAnimation>().animation.GetSize().ToSFMLVec2());
	boundBox.setSize(sf::Vector2f(boundBox.getSize().x * 3, boundBox.getSize().y * 3));
	boundBox.setOrigin(boundBox.getSize().x / 2, boundBox.getSize().y / 2);
	boundBox.setFillColor(sf::Color::Red);
	player->AddComponent<CBoundingBox>(boundBox);

	player->AddComponent<CCollision>();

	player->AddComponent<CInput>();
	player->GetComponent<CInput>().canAttack = true;

	player->AddComponent<CHealth>(5);
}

void Scene_Play::SpawnMeat()
{
	EntityPointer pMeat = m_entityMgr.addEntity(eEntityTag::Item);

	pMeat->AddComponent<CAnimation>(m_pGame->GetAssets().GetAnimation(eAsset::Meat));

	pMeat->AddComponent<CTransform>();
	pMeat->GetComponent<CTransform>().scale = Vec2(4, 4);
	pMeat->GetComponent<CTransform>().pos = GridToMidPixel(0, 0, 3, 5, pMeat);


	sf::RectangleShape boundBox = sf::RectangleShape(pMeat->GetComponent<CAnimation>().animation.GetSize().ToSFMLVec2());
	boundBox.setSize(sf::Vector2f(boundBox.getSize().x * 3, boundBox.getSize().y * 3));
	boundBox.setOrigin(boundBox.getSize().x / 2, boundBox.getSize().y / 2);
	boundBox.setFillColor(sf::Color::Red);
	pMeat->AddComponent<CBoundingBox>(boundBox);

	pMeat->AddComponent<CCollision>();
}

void Scene_Play::DoAction(Action action)
{
	if (m_entityMgr.getPlayer() == nullptr) { return; } //if players dead dont process input

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
			m_drawTextures = !m_drawTextures;
			std::cout << "ToggleTexture action\n";
			break;
		case eAction::ToggleCollision:
			m_drawCollision = !m_drawCollision;
			std::cout << "ToggleCollision action\n";
			break;
		case eAction::ToggleGrid:
			m_drawGrid = !m_drawGrid;
			break;
		case eAction::ToggleCamera:
			m_linkCam = !m_linkCam;
			break;
		case eAction::MoveUp:
			m_entityMgr.getPlayer()->GetComponent<CInput>().up = true;
			break;
		case eAction::MoveDown:
			m_entityMgr.getPlayer()->GetComponent<CInput>().down = true;
			break;
		case eAction::MoveLeft:
			m_entityMgr.getPlayer()->GetComponent<CInput>().left = true;
			break;
		case eAction::MoveRight:
			m_entityMgr.getPlayer()->GetComponent<CInput>().right = true;
			break;
		case eAction::Attack:
			if (m_entityMgr.getPlayer()->GetComponent<CInput>().canAttack)
			{
				SpawnSword();
			}
		case eAction::ActionCount:
			std::cout << "ActionCount (not a valid action)\n";
			break;
		}
	}

	if (action.GetType() == eActionType::Stop)
	{
		switch (action.GetName())
		{
		case eAction::MoveUp:
			m_entityMgr.getPlayer()->GetComponent<CInput>().up = false;
			break;
		case eAction::MoveDown:
			m_entityMgr.getPlayer()->GetComponent<CInput>().down = false;
			break;
		case eAction::MoveLeft:
			m_entityMgr.getPlayer()->GetComponent<CInput>().left = false;
			break;
		case eAction::MoveRight:
			m_entityMgr.getPlayer()->GetComponent<CInput>().right = false;
			break;
		}
	}
}

void Scene_Play::Update()
{
	m_entityMgr.Update();
	sRespawn();
	sEnemyBehavior();
	sMovement();
	sCollision();
	sCombat();
	sCamera();
	sAnimation();
	sRender();
}

void Scene_Play::sDoAction()
{

}

void Scene_Play::sRender()
{
	m_pWindow->clear(sf::Color(0xfc,0xd8,0xa8));
	EntityList allEntities = *(m_entityMgr.getEntities());
	for (EntityPointer e : allEntities)
	{
		if (m_drawTextures && e->HasComponent<CAnimation>())
		{
			e->GetComponent<CAnimation>().animation.Update();
			if (e->GetComponent<CAnimation>().animation.IsEnded())
			{
				if (e->GetComponent<CAnimation>().animation.GetName() == eAsset::Sword)
				{
					m_entityMgr.getPlayer()->GetComponent<CInput>().canAttack = true;
				}
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
			else
			{
				e->GetComponent<CAnimation>().animation.GetSprite().setColor(sf::Color(255, 255, 255, 255));
			}
			m_pWindow->draw(e->GetComponent<CAnimation>().animation.GetSprite());
		}

		if (m_drawCollision)
		{
			if (e->HasComponent<CBoundingBox>() && e->HasComponent<CCollision>())
			{
				Vec2 pos = e->GetComponent<CTransform>().pos;
				e->GetComponent<CBoundingBox>().box.setPosition(pos.x, pos.y);
				e->GetComponent<CBoundingBox>().box.setRotation(e->GetComponent<CTransform>().angle);
				m_pWindow->draw(e->GetComponent<CBoundingBox>().box);
			}
		}
	}

	DrawHud();

	if (m_drawGrid)
	{
		DrawGrid();
	}

	m_pWindow->display();
}

void Scene_Play::DrawHud()
{
	EntityPointer pPlayer = m_entityMgr.getPlayer();
	int hp = 0;
	if (pPlayer != nullptr)
	{
		hp = pPlayer->GetComponent<CHealth>().health;
	}
	// Create a text object
	sf::Text text;
	text.setFont(m_pGame->GetAssets().GetFont(eAsset::SerifFont)); // Set the font
	text.setString("HP: " + std::to_string(hp)); // Set the string to display
	text.setCharacterSize(32); // Set the character size in pixels
	text.setFillColor(sf::Color::Red); // Set the fill color
	text.setPosition(50, 50); // Set the position of the text
	text.setOutlineColor(sf::Color::Black);
	text.setOutlineThickness(2);

	sf::View worldView = m_pWindow->getView();
	sf::View hudView(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
	m_pWindow->setView(hudView);
	m_pWindow->draw(text);
	m_pWindow->setView(worldView);
}

void Scene_Play::DrawGrid()
{
	//TODO: calculate what grids to draw depending on where view is
	sf::View view = m_pWindow->getView();
	int numXGrids = ((view.getCenter().x + view.getSize().x / 2) / GRID_SIZE_PIXELS) + 1;
	int numYGrids = ((view.getCenter().y + view.getSize().y / 2) / GRID_SIZE_PIXELS) + 1;

	sf::Vertex verticesX[2 * 500];//figure out good size for this array
	sf::Vertex verticesY[2 * 500];

	int j = 0;
	for (int i = 0; i < numXGrids; i++)
	{
		//draw the x grids first (vertical lines)
		verticesX[j] = sf::Vertex(sf::Vector2f(i * GRID_SIZE_PIXELS, 0), sf::Color::Black);
		verticesX[j + 1] = sf::Vertex(sf::Vector2f(i * GRID_SIZE_PIXELS, WINDOW_HEIGHT), sf::Color::Black);
		j += 2;
	}
	j = 0;
	for(int i = 0; i < numYGrids; i++)
	{
		verticesY[j] = sf::Vertex(sf::Vector2f(0, i * GRID_SIZE_PIXELS), sf::Color::Black);
		verticesY[j + 1] = sf::Vertex(sf::Vector2f(WINDOW_WIDTH, i * GRID_SIZE_PIXELS), sf::Color::Black);
		j += 2;
	}
	m_pWindow->draw(verticesX, sizeof(verticesX) / sizeof(sf::Vertex), sf::Lines);
	m_pWindow->draw(verticesY, sizeof(verticesY) / sizeof(sf::Vertex), sf::Lines);
}

void Scene_Play::sMovement()
{
	EntityPointer player = m_entityMgr.getPlayer();
	if (player)
	{
		if (player->GetComponent<CInput>().has && player->GetComponent<CTransform>().has)
		{
			Vec2 oldVelocity = player->GetComponent<CTransform>().velocity;
			player->GetComponent<CTransform>().velocity = Vec2(0, 0);
			if (player->GetComponent<CInput>().left)
			{
				player->GetComponent<CTransform>().velocity.x -= PLAYER_SPEED;
			}
			if (player->GetComponent<CInput>().right)
			{
				player->GetComponent<CTransform>().velocity.x += PLAYER_SPEED;
			}
			if (player->GetComponent<CInput>().up)
			{
				player->GetComponent<CTransform>().velocity.y -= PLAYER_SPEED;
			}
			if (player->GetComponent<CInput>().down)
			{
				player->GetComponent<CTransform>().velocity.y += PLAYER_SPEED;
			}

			if ( (player->GetComponent<CTransform>().velocity.x != 0 && player->GetComponent<CTransform>().velocity.y != 0) ) //dont let player move in x and y direction at once
			{
				player->GetComponent<CTransform>().velocity = oldVelocity;
			}

			player->GetComponent<CTransform>().prevPos = player->GetComponent<CTransform>().pos;
			player->GetComponent<CTransform>().pos.x += player->GetComponent<CTransform>().velocity.x;
			player->GetComponent<CTransform>().pos.y += player->GetComponent<CTransform>().velocity.y;
		}
	}

	EntityList enemies = *(m_entityMgr.getEntities(eEntityTag::Enemy));
	for (EntityPointer enemy : enemies)
	{
		enemy->GetComponent<CTransform>().prevPos = enemy->GetComponent<CTransform>().pos;
		enemy->GetComponent<CTransform>().pos.x += enemy->GetComponent<CTransform>().velocity.x;
		enemy->GetComponent<CTransform>().pos.y += enemy->GetComponent<CTransform>().velocity.y;
	}
}

void Scene_Play::sCollision()
{
	EntityPointer pPlayer = m_entityMgr.getPlayer();
	EntityList attacks = *(m_entityMgr.getEntities(eEntityTag::Attack));
	EntityList tiles = *(m_entityMgr.getEntities(eEntityTag::Tile));
	EntityList enemies = *(m_entityMgr.getEntities(eEntityTag::Enemy));
	EntityList items = *(m_entityMgr.getEntities(eEntityTag::Item));

	for (EntityPointer pTile : tiles)
	{
		Vec2 overlap = DetectOverlap(pPlayer, pTile);
		if (overlap.x > 0 && overlap.y > 0)
		{
			ResolveCollision(pPlayer, pTile);
		}
	}

	for (EntityPointer enemy : enemies)
	{
		for (EntityPointer pTile : tiles)
		{
			Vec2 overlap = DetectOverlap(enemy, pTile);
			if (overlap.x > 0 && overlap.y > 0)
			{
				ResolveCollision(enemy, pTile);
			}
		}

		Vec2 overlap = DetectOverlap(enemy, pPlayer);
		if (overlap.x > 0 && overlap.y > 0)
		{
			ResolveEnemyPlayerCollision(enemy, pPlayer);
		}
	}

	for (EntityPointer pAttack : attacks)
	{
		if (pAttack->GetComponent<CCollision>().has == false) { continue; }
		for (EntityPointer pEnemy : enemies)
		{
			Vec2 overlap = DetectOverlap(pAttack, pEnemy);
			if (overlap.x > 0 && overlap.y > 0)
			{
				ResolveAttackCollision(pAttack, pEnemy);
			}
		}
	}

	for (EntityPointer pItem : items)
	{
		if (pItem->GetComponent<CCollision>().has == false) { continue; }
		Vec2 overlap = DetectOverlap(pItem, pPlayer);
		if (overlap.x > 0 && overlap.y > 0)
		{
			ResolveItemPickUp(pItem);
		}
	}
}

void Scene_Play::ResolveItemPickUp(EntityPointer pItem)
{
	EntityPointer pPlayer = m_entityMgr.getPlayer();
	pPlayer->GetComponent<CHealth>().health += 1;
	pItem->markInactive();
}

void Scene_Play::ResolveAttackCollision(EntityPointer pAttack, EntityPointer pTarget)
{
	if (pAttack->GetComponent<CDamage>().has && pTarget->GetComponent<CHealth>().has)
	{
		pTarget->GetComponent<CHealth>().health -= pAttack->GetComponent<CDamage>().damage;
		pAttack->GetComponent<CCollision>().has = false;

		if (pTarget->GetComponent<CHealth>().health <= 0)
		{
			pTarget->markInactive();
		}
	}
}

void Scene_Play::ResolveEnemyPlayerCollision(EntityPointer pEnemy, EntityPointer pPlayer)
{
	if (pPlayer->GetComponent<CInvincibility>().duration > 0) { return; } //skip if player has i frames
	pPlayer->GetComponent<CHealth>().health -= 1;
	pPlayer->GetComponent<CInvincibility>().duration = NUM_I_FRAMES;
	if (pPlayer->GetComponent<CHealth>().health <= 0)
	{
		pPlayer->markInactive();
	}
}

void Scene_Play::sCamera()
{
	EntityPointer player = m_entityMgr.getPlayer();
	if (player)
	{
		sf::Vector2f cameraOrigin;
		if (m_linkCam)
		{
			cameraOrigin = sf::Vector2f(player->GetComponent<CTransform>().pos.x, player->GetComponent<CTransform>().pos.y);
		}
		else
		{
			sf::Vector2i roomIdx = GetRoom(player->GetComponent<CTransform>().pos);
			float roomOffsetX = roomIdx.x * WINDOW_WIDTH;
			float roomOffsetY = roomIdx.y * WINDOW_HEIGHT;
			cameraOrigin = sf::Vector2f(roomOffsetX + (WINDOW_WIDTH/2), roomOffsetY + WINDOW_HEIGHT/2);
		}

		sf::Vector2f cameraSize = sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT);
		sf::View view(cameraOrigin, cameraSize);
		m_pGame->UpdateView(view);
	}

}

void Scene_Play::sCombat()
{
	EntityPointer pPlayer = m_entityMgr.getPlayer();
	if (pPlayer == nullptr) { return; }

	if (pPlayer->GetComponent<CInvincibility>().duration > 0)
	{
		pPlayer->GetComponent<CInvincibility>().duration -= 1;
	}

	EntityList allEntities = *(m_entityMgr.getEntities());
	for (EntityPointer e : allEntities)
	{
		if (e->getTag() == eEntityTag::Attack)
		{
			eAsset attackName = e->GetComponent<CAnimation>().animation.GetName();
			switch(attackName)
			{
			case eAsset::Sword:
			{
				UpdateSwordTransform(e);
				break;
			}
			default:
				break;
			}
		}
	}
}

void Scene_Play::UpdateSwordTransform(EntityPointer pSword)
{

	EntityPointer pPlayer = m_entityMgr.getPlayer();
	Vec2 offset = Vec2(0,0);
	sf::Vector2f swordSize = pSword->GetComponent<CBoundingBox>().box.getSize();
	pSword->GetComponent<CTransform>().scale = Vec2(4, -4);
	switch (pPlayer->GetComponent<CAnimation>().animation.GetName())
	{
	case eAsset::LinkWalkUp:
	case eAsset::LinkIdleUp:
		pSword->GetComponent<CTransform>().angle = 180.0;
		//offset.y = -1 * (swordSize.y / 2 + pPlayer->GetComponent<CBoundingBox>().box.getSize().y / 2);
		offset.y = -1 * (swordSize.y / 2);
		offset.x = 15;
		break;
	case eAsset::LinkWalkDown:
	case eAsset::LinkIdleDown:
		offset.y = (swordSize.y / 2);
		offset.x = 15;
		break;
	case eAsset::LinkWalkSide:
	case eAsset::LinkIdleSide:
		pSword->GetComponent<CTransform>().angle = -90.0;
		offset.y = 6;
		offset.x = (10 + swordSize.x / 2);
		if (pPlayer->GetComponent<CTransform>().scale.x < 0)
		{
			offset.x *= -1;
			pSword->GetComponent<CTransform>().angle *= -1;

		}
		break;
	default:
		break;
	}

	pSword->GetComponent<CTransform>().pos = pPlayer->GetComponent<CTransform>().pos + offset;
}

sf::Vector2i Scene_Play::GetRoom(Vec2 pos)
{
	int roomX = (int)(pos.x / (float)(WINDOW_WIDTH));
	if (pos.x < 0)
	{
		roomX -= 1;
	}
	int roomY = (int)(pos.y / (float)(WINDOW_HEIGHT));
	if (pos.y < 0)
	{
		roomY -= 1;
	}
	return sf::Vector2i(roomX, roomY);
}

Vec2 Scene_Play::DetectOverlap(EntityPointer a, EntityPointer b)
{
	if (a == nullptr || b == nullptr) { return Vec2(0, 0); }
	if (!a->GetComponent<CTransform>().has || !a->GetComponent<CBoundingBox>().has || !a->GetComponent<CCollision>().has)
	{
		return Vec2(0, 0);
	}
	if (!b->GetComponent<CTransform>().has || !b->GetComponent<CBoundingBox>().has || !b->GetComponent<CCollision>().has)
	{
		return Vec2(0, 0);
	}
	Vec2 posA = a->GetComponent<CTransform>().pos;
	Vec2 boxA = Vec2(a->GetComponent<CBoundingBox>().box.getSize().x, a->GetComponent<CBoundingBox>().box.getSize().y);
	Vec2 posB = b->GetComponent<CTransform>().pos;
	Vec2 boxB = Vec2(b->GetComponent<CBoundingBox>().box.getSize().x, b->GetComponent<CBoundingBox>().box.getSize().y);

	Vec2 delta = Vec2(posA.x - posB.x, posA.y - posB.y);
	if (delta.x < 0) { delta.x *= -1; }
	if (delta.y < 0) { delta.y *= -1; }

	float overlapX = (boxA.x / 2) + (boxB.x / 2) - delta.x;
	float overlapY = (boxA.y / 2) + (boxB.y / 2) - delta.y;
	if (overlapX < 0) { overlapX = 0; }
	if (overlapY < 0) { overlapY = 0; }

	return Vec2(overlapX, overlapY);
}

Vec2 Scene_Play::DetectPreviousOverlap(EntityPointer a, EntityPointer b)
{
	if (!a->GetComponent<CTransform>().has || !a->GetComponent<CBoundingBox>().has || !a->GetComponent<CCollision>().has)
	{
		return Vec2(0, 0);
	}
	if (!b->GetComponent<CTransform>().has || !b->GetComponent<CBoundingBox>().has || !b->GetComponent<CCollision>().has)
	{
		return Vec2(0, 0);
	}
	Vec2 posA = a->GetComponent<CTransform>().prevPos;
	Vec2 boxA = Vec2(a->GetComponent<CBoundingBox>().box.getSize().x, a->GetComponent<CBoundingBox>().box.getSize().y);
	Vec2 posB = b->GetComponent<CTransform>().prevPos;
	Vec2 boxB = Vec2(b->GetComponent<CBoundingBox>().box.getSize().x, b->GetComponent<CBoundingBox>().box.getSize().y);

	Vec2 delta = Vec2(posA.x - posB.x, posA.y - posB.y);
	if (delta.x < 0) { delta.x *= -1; }
	if (delta.y < 0) { delta.y *= -1; }

	float overlapX = (boxA.x / 2) + (boxB.x / 2) - delta.x;
	float overlapY = (boxA.y / 2) + (boxB.y / 2) - delta.y;
	if (overlapX < 0) { overlapX = 0; }
	if (overlapY < 0) { overlapY = 0; }

	return Vec2(overlapX, overlapY);
}

void Scene_Play::ResolveCollision(EntityPointer a, EntityPointer b)
{
	Vec2 prevOverlap = DetectPreviousOverlap(a, b);
	Vec2 posA = a->GetComponent<CTransform>().pos;
	Vec2 boxA = Vec2(a->GetComponent<CBoundingBox>().box.getSize().x, a->GetComponent<CBoundingBox>().box.getSize().y);
	Vec2 posB = b->GetComponent<CTransform>().pos;
	Vec2 boxB = Vec2(b->GetComponent<CBoundingBox>().box.getSize().x, b->GetComponent<CBoundingBox>().box.getSize().y);
	Vec2 newPosA = posA;

	if (prevOverlap.y > 0) //movement came from the side
	{
		if (posA.x > posB.x) //a is to the right of b
		{
			newPosA.x = posB.x + (boxB.x / 2) + (boxA.x / 2);
		}
		else
		{
			newPosA.x = posB.x - (boxB.x / 2) - (boxA.x / 2);
		}
	}
	else
	{
		if (posA.y > posB.y) //a is below b
		{
			newPosA.y = posB.y + (boxB.y / 2) + (boxA.y / 2);
		}
		else
		{
			newPosA.y = posB.y - (boxB.y / 2) - (boxA.y / 2);
		}
	}

	a->GetComponent<CTransform>().pos = newPosA;
}

void Scene_Play::sRespawn()
{
	if (m_entityMgr.getPlayer() == nullptr)
	{
		SpawnPlayer();
	}
}

void Scene_Play::sAnimation()
{
	EntityPointer pPlayer = m_entityMgr.getPlayer();
	if (pPlayer == nullptr) { return; }
	Assets assets = m_pGame->GetAssets();
	Vec2 velocity = pPlayer->GetComponent<CTransform>().velocity;
	eAsset newState;
	switch(pPlayer->GetComponent<CAnimation>().animation.GetName())
	{
	case eAsset::LinkWalkUp:
	case eAsset::LinkIdleUp:
		pPlayer->GetComponent<CTransform>().scale.x = 4;
		if (velocity == Vec2(0, 0))
		{
			newState = eAsset::LinkIdleUp;
		}
		else if (velocity.y > 0)
		{
			newState = eAsset::LinkWalkDown;
		}
		else if (velocity.y < 0)
		{
			newState = eAsset::LinkWalkUp;
		}
		else if (velocity.x > 0)
		{
			newState = eAsset::LinkWalkSide;
			pPlayer->GetComponent<CTransform>().scale.x = 4;
		}
		else if (velocity.x < 0)
		{
			newState = eAsset::LinkWalkSide;
			pPlayer->GetComponent<CTransform>().scale.x = -4;
		}
		break;
	case eAsset::LinkWalkDown:
	case eAsset::LinkIdleDown:
		pPlayer->GetComponent<CTransform>().scale.x = 4;
		if (velocity == Vec2(0, 0))
		{
			newState = eAsset::LinkIdleDown;
		}
		else if (velocity.y > 0)
		{
			newState = eAsset::LinkWalkDown;
		}
		else if (velocity.y < 0)
		{
			newState = eAsset::LinkWalkUp;
		}
		else if (velocity.x > 0)
		{
			newState = eAsset::LinkWalkSide;
			pPlayer->GetComponent<CTransform>().scale.x = 4;
		}
		else if (velocity.x < 0)
		{
			newState = eAsset::LinkWalkSide;
			pPlayer->GetComponent<CTransform>().scale.x = -4;
		}
		break;
	case eAsset::LinkWalkSide:
	case eAsset::LinkIdleSide:
		if (velocity == Vec2(0, 0))
		{
			newState = eAsset::LinkIdleSide;
		}
		else if (velocity.y > 0)
		{
			newState = eAsset::LinkWalkDown;
		}
		else if (velocity.y < 0)
		{
			newState = eAsset::LinkWalkUp;
		}
		else if (velocity.x > 0)
		{
			newState = eAsset::LinkWalkSide;
			pPlayer->GetComponent<CTransform>().scale.x = 4;
		}
		else if (velocity.x < 0)
		{
			newState = eAsset::LinkWalkSide;
			pPlayer->GetComponent<CTransform>().scale.x = -4;
		}
		break;
	default:
		break;
	}
	if (newState != pPlayer->GetComponent<CAnimation>().animation.GetName())
	{
		int state = (int)(newState);
		std::cout << "State:" << state << std::endl;
		pPlayer->GetComponent<CAnimation>().animation = assets.GetAnimation(newState);
	}
}

void Scene_Play::SpawnEnemy(std::string enemyConfig)
{
	eAsset enemyType = eAsset::AssetCount;
	std::string enemyTypeStr = GetAndDeleteNextStringToken(enemyConfig);
	auto stringToEnemyMapEntry = m_charToEnemyMap.find(enemyTypeStr);
	if (stringToEnemyMapEntry != m_charToEnemyMap.end())
	{
		enemyType = stringToEnemyMapEntry->second;
	}

	int roomX = stoi(GetAndDeleteNextStringToken(enemyConfig));
	int roomY = stoi(GetAndDeleteNextStringToken(enemyConfig));
	int tileX = stoi(GetAndDeleteNextStringToken(enemyConfig));
	std::string tileystring = GetAndDeleteNextStringToken(enemyConfig);
	int tileY = stoi(tileystring);
	std::string behavior = GetAndDeleteNextStringToken(enemyConfig);

	auto enemy = m_entityMgr.addEntity(eEntityTag::Enemy);
	enemy->AddComponent<CAnimation>(m_pGame->GetAssets().GetAnimation(enemyType));
	enemy->AddComponent<CTransform>();
	enemy->GetComponent<CTransform>().scale = Vec2(4, 4);
	enemy->GetComponent<CTransform>().pos = GridToMidPixel(roomX, roomY, tileX, tileY, enemy);
	enemy->GetComponent<CTransform>().prevPos = enemy->GetComponent<CTransform>().pos;
	sf::RectangleShape boundBox = sf::RectangleShape(enemy->GetComponent<CAnimation>().animation.GetSize().ToSFMLVec2());
	boundBox.setSize(sf::Vector2f(boundBox.getSize().x * 3, boundBox.getSize().y * 3));
	boundBox.setOrigin(boundBox.getSize().x / 2, boundBox.getSize().y / 2);
	boundBox.setFillColor(sf::Color::Red);
	enemy->AddComponent<CBoundingBox>(boundBox);
	enemy->AddComponent<CHealth>(3);
	enemy->AddComponent<CDamage>(1);
	enemy->AddComponent<CCollision>();
	std::vector<Vec2> positions =
	{
		Vec2(8,8),
		Vec2(4,8),
		Vec2(4,4),
		Vec2(8,4)
	};
	
	eBehavior behave;
	switch (behavior[0])
	{
	case 'P':
		behave = eBehavior::Patrol;
		break;
	case 'C':
	default:
		behave = eBehavior::Chase;
		break;
	}
	enemy->AddComponent<CBehavior>(behave, positions);
}

void Scene_Play::sEnemyBehavior()
{
	EntityList enemies = *(m_entityMgr.getEntities(eEntityTag::Enemy));
	for (EntityPointer enemy : enemies)
	{
		CBehavior cBehavior = enemy->GetComponent<CBehavior>();
		if (cBehavior.has)
		{
			switch (cBehavior.behavior)
			{
			case eBehavior::Patrol:
				PatrolBehavior(enemy);
				break;
			case eBehavior::Chase:
				ChaseBehavior(enemy);
				break;
			default:
				break;
			}
		}
	}
}

void Scene_Play::PatrolBehavior(EntityPointer e)
{
	CBehavior cBehavior = e->GetComponent<CBehavior>();
	Vec2 target = GridToMidPixel(0,0,cBehavior.positions[cBehavior.posIdx].x, cBehavior.positions[cBehavior.posIdx].y, e);
	Vec2 posDelta = e->GetComponent<CTransform>().pos - target;
	float speed = e->GetComponent<CTransform>().velocity.Absolute();

	if (posDelta <= speed && posDelta >= -speed)
	{
		int posCount = cBehavior.positions.size();
		e->GetComponent<CBehavior>().posIdx = (cBehavior.posIdx + 1) % posCount;
		target = GridToMidPixel(0, 0, cBehavior.positions[cBehavior.posIdx].x, cBehavior.positions[cBehavior.posIdx].y, e);
	}
	Vec2 newSpeed = target - e->GetComponent<CTransform>().pos;
	newSpeed.Normalize(ENEMY_CHASE_SPEED);
	e->GetComponent<CTransform>().velocity = newSpeed;
}

void Scene_Play::ChaseBehavior(EntityPointer e)
{
	EntityPointer pPlayer = m_entityMgr.getPlayer();
	if (pPlayer == nullptr) { return; }
	Vec2 playerPos = pPlayer->GetComponent<CTransform>().pos;
	Vec2 entityPos = e->GetComponent<CTransform>().pos;
	if (PathClear(entityPos, playerPos))
	{
		Vec2 pathToPlayer = playerPos - entityPos;
		pathToPlayer.Normalize(ENEMY_CHASE_SPEED);
		e->GetComponent<CTransform>().velocity = pathToPlayer;
	}
	else
	{
		e->GetComponent<CTransform>().velocity = Vec2(0,0);
	}
}

bool Scene_Play::LineIntersect(Vec2 a, Vec2 b, Vec2 c, Vec2 d)
{
	Vec2 ab = b - a;
	Vec2 cd = d - c;
	float abxcd = ab.Cross(cd);
	Vec2 cma = c - a;
	float t = (cma.Cross(cd)) / abxcd;
	float u = (cma.Cross(ab)) / abxcd;
	if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
	{
		return true;
	}
	return false;
}

bool Scene_Play::EntityIntersect(EntityPointer e,  Vec2 a, Vec2 b)
{
	Vec2 entityPos = e->GetComponent<CTransform>().pos;
	Vec2 halfBoxSize = Vec2(e->GetComponent<CBoundingBox>().box.getSize().x/2, e->GetComponent<CBoundingBox>().box.getSize().y/2);
	Vec2 entityVertices[4] =
	{
		Vec2(entityPos.x - halfBoxSize.x, entityPos.y - halfBoxSize.y),
		Vec2(entityPos.x + halfBoxSize.x, entityPos.y - halfBoxSize.y),
		Vec2(entityPos.x + halfBoxSize.x, entityPos.y + halfBoxSize.y),
		Vec2(entityPos.x - halfBoxSize.x, entityPos.y + halfBoxSize.y),
	};

	for (size_t i = 0; i < 4; i++)
	{
		Vec2 c = entityVertices[i];
		Vec2 d;
		if (i == 3)
		{
			d = entityVertices[0];
		}
		else
		{
			d = entityVertices[i+1];
		}
		if (LineIntersect(a, b, c, d))
		{
			return true;
		}
	}
	return false;
}

bool Scene_Play::PathClear(Vec2 a, Vec2 b)
{
	EntityList allEntities = *(m_entityMgr.getEntities());
	for (EntityPointer e : allEntities)
	{
		if (e->GetComponent<CBlockVision>().has)
		{
			if (EntityIntersect(e, a, b))
			{
				return false;
			}
		}
	}
	return true;
}