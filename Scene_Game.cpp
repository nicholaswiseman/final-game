#include "Scene_Game.h"

Scene_Game::Scene_Game(const std::string& levelPath, sf::RenderWindow* pWindow, GameEngine* pGame)
	: Scene(pWindow, pGame)
	, m_levelPath(levelPath)
{
	Initialize(m_levelPath);
}

void Scene_Game::Initialize(const std::string& levelPath)
{
	CreateBackground();
	RegisterSceneActions();
	SpawnPlayer();
	//LoadLevel(levelPath);
}

void Scene_Game::CreateBackground()
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

	EntityPointer earth = m_entityMgr.addEntity(eEntityTag::Decoration);
	earth->AddComponent<CAnimation>(m_pGame->GetAssets().GetAnimation(eAsset::Earth));
	earth->AddComponent<CTransform>();
	earth->GetComponent<CTransform>().pos = Vec2(700, 100);
	earth->AddComponent<CParallax>(0.5);
}

void Scene_Game::RegisterSceneActions()
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
}

void Scene_Game::Update()
{
	m_entityMgr.Update();
	sCamera();
	sMovement();
	sBackgroundScroll();
	sParallax();
	sRender();
}

void Scene_Game::sCamera()
{
	sf::Vector2f cameraSize = sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT);
	sf::View oldView = m_pWindow->getView();
	sf::Vector2f newOrigin = sf::Vector2f(oldView.getCenter().x, oldView.getCenter().y - SCREEN_SCROLL_SPEED);
	sf::View view(newOrigin, cameraSize);
	m_pGame->UpdateView(view);
}

void Scene_Game::SpawnPlayer()
{
	auto player = m_entityMgr.addEntity(eEntityTag::Player);

	player->AddComponent<CAnimation>(m_pGame->GetAssets().GetAnimation(eAsset::PlayerShip));

	player->AddComponent<CTransform>();
	//player->GetComponent<CTransform>().scale = Vec2(4, 4);
	player->GetComponent<CTransform>().pos = Vec2(100, 100);//GridToMidPixel(0, 0, 8, 5, player);

	sf::RectangleShape boundBox = sf::RectangleShape(player->GetComponent<CAnimation>().animation.GetSize().ToSFMLVec2());
	boundBox.setSize(sf::Vector2f(boundBox.getSize().x, boundBox.getSize().y));
	boundBox.setOrigin(boundBox.getSize().x / 2, boundBox.getSize().y / 2);
	boundBox.setFillColor(sf::Color::Red);
	player->AddComponent<CBoundingBox>(boundBox);

	player->AddComponent<CCollision>();

	player->AddComponent<CInput>();
	player->GetComponent<CInput>().canAttack = true;

	player->AddComponent<CHealth>(5);
}

void Scene_Game::sRender()
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
				if (e->GetComponent<CAnimation>().animation.GetName() == eAsset::Sword)
				{
					m_entityMgr.getPlayer()->GetComponent<CInput>().canAttack = true;
				}
				e->markInactive();
			}
			CTransform transform = e->GetComponent<CTransform>();
			e->GetComponent<CAnimation>().animation.GetSprite().setPosition(transform.pos.x, transform.pos.y);
			e->GetComponent<CAnimation>().animation.GetSprite().setRotation(transform.angle);
			//e->GetComponent<CAnimation>().animation.GetSprite().setScale(transform.scale.x, transform.scale.y);
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

	//DrawHud();

	//if (m_drawGrid)
	//{
	//	DrawGrid();
	//}

	m_pWindow->display();
}

void Scene_Game::RenderBackground()
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

void Scene_Game::sDoAction()
{
}

void Scene_Game::DoAction(Action action)
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
			break;
		case eAction::ToggleCollision:
			m_drawCollision = !m_drawCollision;
			break;
		case eAction::ToggleGrid:
			//m_drawGrid = !m_drawGrid;
			break;
		case eAction::ToggleCamera:
			//m_linkCam = !m_linkCam;
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
				SpawnBullet();
			}
		case eAction::ActionCount:
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

void Scene_Game::sMovement()
{
	UpdatePlayerMovement();

	EntityList enemies = *(m_entityMgr.getEntities(eEntityTag::Enemy));
	for (EntityPointer enemy : enemies)
	{
		enemy->GetComponent<CTransform>().prevPos = enemy->GetComponent<CTransform>().pos;
		enemy->GetComponent<CTransform>().pos.x += enemy->GetComponent<CTransform>().velocity.x;
		enemy->GetComponent<CTransform>().pos.y += enemy->GetComponent<CTransform>().velocity.y;
	}
	EntityList attacks = *(m_entityMgr.getEntities(eEntityTag::Attack));
	for (EntityPointer attack : attacks)
	{
		attack->GetComponent<CTransform>().prevPos = attack->GetComponent<CTransform>().pos;
		attack->GetComponent<CTransform>().pos.x += attack->GetComponent<CTransform>().velocity.x;
		attack->GetComponent<CTransform>().pos.y += attack->GetComponent<CTransform>().velocity.y;
	}
}

void Scene_Game::sParallax()
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
void Scene_Game::UpdatePlayerMovement()
{
	EntityPointer player = m_entityMgr.getPlayer();
	if (player)
	{
		if (player->GetComponent<CInput>().has && player->GetComponent<CTransform>().has)
		{
			Vec2 oldVelocity = player->GetComponent<CTransform>().velocity;
			float oldAngle = player->GetComponent<CTransform>().angle;
			player->GetComponent<CTransform>().velocity = Vec2(0, 0);
			if (player->GetComponent<CInput>().left)
			{
				player->GetComponent<CTransform>().velocity.x -= PLAYER_WALK_SPEED;
				//player->GetComponent<CTransform>().angle = -90;
			}
			if (player->GetComponent<CInput>().right)
			{
				player->GetComponent<CTransform>().velocity.x += PLAYER_WALK_SPEED;
				//player->GetComponent<CTransform>().angle = 90;
			}
			if (player->GetComponent<CInput>().up)
			{
				player->GetComponent<CTransform>().velocity.y -= PLAYER_WALK_SPEED;
				//player->GetComponent<CTransform>().angle = 0;
			}
			if (player->GetComponent<CInput>().down)
			{
				player->GetComponent<CTransform>().velocity.y += PLAYER_WALK_SPEED;
				//player->GetComponent<CTransform>().angle = 180;
			}

			if ((player->GetComponent<CTransform>().velocity.x != 0 && player->GetComponent<CTransform>().velocity.y != 0)) //dont let player move in x and y direction at once
			{
				player->GetComponent<CTransform>().velocity = oldVelocity;
				player->GetComponent<CTransform>().angle = oldAngle;
			}

			player->GetComponent<CTransform>().prevPos = player->GetComponent<CTransform>().pos;
			player->GetComponent<CTransform>().pos.x += player->GetComponent<CTransform>().velocity.x;
			player->GetComponent<CTransform>().pos.y += player->GetComponent<CTransform>().velocity.y;
			KeepEntityInWindow(player);
		}
	}
}

void Scene_Game::KeepEntityInWindow(EntityPointer e)
{
	Vec2 currentPos = e->GetComponent<CTransform>().pos;
	Vec2 entitySize = e->GetComponent<CAnimation>().animation.GetSize();
	float entityHalfWidth = entitySize.x / 2.0f;
	float entityHalfHeight = entitySize.y / 2.0f;
	sf::View view = m_pWindow->getView();
	sf::Vector2f viewCenter = view.getCenter();
	sf::Vector2f viewSize = view.getSize();
	float leftBoundary = viewCenter.x - viewSize.x / 2.0f + entityHalfWidth;
	float rightBoundary = viewCenter.x + viewSize.x / 2.0f - entityHalfWidth;
	float topBoundary = viewCenter.y - viewSize.y / 2.0f + entityHalfHeight;
	float bottomBoundary = viewCenter.y + viewSize.y / 2.0f - entityHalfHeight;

	if (currentPos.x > rightBoundary)
	{
		e->GetComponent<CTransform>().pos.x = rightBoundary;
	}
	if (currentPos.x < leftBoundary)
	{
		e->GetComponent<CTransform>().pos.x = leftBoundary;
	}
	if (currentPos.y > bottomBoundary)
	{
		e->GetComponent<CTransform>().pos.y = bottomBoundary;
	}
	if (currentPos.y < topBoundary)
	{
		e->GetComponent<CTransform>().pos.y = topBoundary;
	}
}

void Scene_Game::SpawnBullet()
{
	Vec2 spawnPoint = CalculateBulletSpawnPoint();
	Vec2 bulletVelocity = CalculateBulletSpeed();

	EntityPointer pBullet = m_entityMgr.addEntity(eEntityTag::Attack);

	pBullet->AddComponent<CAnimation>(m_pGame->GetAssets().GetAnimation(eAsset::Bullet));

	pBullet->AddComponent<CTransform>();
	pBullet->GetComponent<CTransform>().pos = spawnPoint;
	pBullet->GetComponent<CTransform>().angle = m_entityMgr.getPlayer()->GetComponent<CTransform>().angle;
	pBullet->GetComponent<CTransform>().velocity = bulletVelocity;

	sf::RectangleShape boundBox = sf::RectangleShape(pBullet->GetComponent<CAnimation>().animation.GetSize().ToSFMLVec2());
	boundBox.setSize(sf::Vector2f(boundBox.getSize().x, boundBox.getSize().y));
	boundBox.setOrigin(boundBox.getSize().x / 2, boundBox.getSize().y / 2);
	boundBox.setFillColor(sf::Color::Red);
	pBullet->AddComponent<CBoundingBox>(boundBox);

	pBullet->AddComponent<CCollision>();
}

Vec2 Scene_Game::CalculateBulletSpawnPoint()
{
	EntityPointer pPlayer = m_entityMgr.getPlayer();
	Vec2 spawnPoint = Vec2(0, 0);
	if (pPlayer)
	{
		Vec2 playerPos = pPlayer->GetComponent<CTransform>().pos;
		float playerHalfSize = pPlayer->GetComponent<CAnimation>().animation.GetSize().x / 2;
		float angle = ConvertTransformRotationToRads(pPlayer->GetComponent<CTransform>().angle);
		Vec2 direction = Vec2(cos(angle), sin(angle)) * playerHalfSize;
		spawnPoint = Vec2(playerPos.x + direction.x, playerPos.y - direction.y);
	}
	return spawnPoint;
}

Vec2 Scene_Game::CalculateBulletSpeed()
{
	EntityPointer pPlayer = m_entityMgr.getPlayer();
	Vec2 speed = Vec2(0, 0);

	if (pPlayer)
	{
		float angle = ConvertTransformRotationToRads(pPlayer->GetComponent<CTransform>().angle);
		Vec2 direction = Vec2(cos(angle), -1*sin(angle));
		speed = direction * 5.0f;
	}

	return speed;
}

float Scene_Game::ConvertTransformRotationToRads(float rotation)
{
	return (-1 * rotation + 90) * acos(-1) / 180; //acos(-1)=pi
}

void Scene_Game::sBackgroundScroll()
{
	EntityList bgs = *(m_entityMgr.getEntities(eEntityTag::Background));
	for (EntityPointer bg : bgs)
	{
		float bgHeight = bg->GetComponent<CAnimation>().animation.GetSize().y;
		float bgCenter = bg->GetComponent<CTransform>().pos.y;
		float topOfBg = bgCenter - bgHeight / 2.0f;
		float cameraHeight = m_pWindow->getView().getSize().y;
		float cameraCenter = m_pWindow->getView().getCenter().y;
		float bottomOfView = cameraCenter + cameraHeight / 2.0f;

		bool isOffScreen = topOfBg > bottomOfView;
		if (isOffScreen)
		{
			bg->GetComponent<CTransform>().pos.y -= 2.0f * bgHeight;
		}
	}
}