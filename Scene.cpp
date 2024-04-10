#include "Scene.h"

Scene::Scene(sf::RenderWindow* pWindow, GameEngine* pGame)
{
	m_pWindow = pWindow;
	m_pGame = pGame;
	m_currentFrame = 0;
}

void Scene::RegisterAction(int input, eAction action)
{
	m_actionMap[input] = action;
}

void Scene::Simulate(int myInt)
{
}

bool Scene::IsEnded() const
{
	return false;
}

ActionMap Scene::GetActionMap()
{
	return m_actionMap;
}
