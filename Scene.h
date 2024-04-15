#pragma once
#include <memory>
#include <string>

#include "EntityManager.h"
#include "Action.h"
#include "GameEngine.h"

typedef std::map<int, eAction> ActionMap;

class Scene
{
protected:
	EntityManager m_entityMgr;
	int m_currentFrame;
	ActionMap m_actionMap;
	bool m_isPaused = false;
	bool m_isEnded = false;
	sf::RenderWindow* m_pWindow;
	GameEngine* m_pGame;

public:
	Scene(sf::RenderWindow* pWindow, GameEngine* pGame);
	virtual void Update() = 0;
	virtual void sDoAction(Action action) = 0;
	virtual void sRender() = 0;

	void Simulate(int num);
	virtual void DoAction(Action action) = 0;
	void RegisterAction(int input, eAction action);

	size_t GetWidth() const;
	size_t GetHeight() const;
	size_t GetCurrentFrame() const;

	bool IsEnded() const;
	ActionMap GetActionMap();
	void DrawLine(const Vec2& p1, const Vec2& p2);

};

