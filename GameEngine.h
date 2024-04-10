#pragma once
#include <map>
#include <memory>

#include "SFML/Graphics.hpp"

#include "eAsset.h"
#include "eScene.h"
#include "Assets.h"

class Scene;

class GameEngine
{
	std::map<eScene, std::shared_ptr<Scene>> m_scenes;
	sf::RenderWindow m_window;
	Assets m_assets;
	eScene m_currentScene;
	bool m_isRunning;
	uint64_t testCount = 0;

	void Initialize(const std::string& assetPath);
	void UpdateScene();
	void Quit();
	void ChangeScene(eScene sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene);
	void sUserInput();

public:
	GameEngine(const std::string& assetPath);
	void Run();
	bool IsRunning();
	Assets& GetAssets();
	const sf::RenderWindow& GetWindow() const;
	std::shared_ptr<Scene> getCurrentScene();
	void UpdateView(sf::View newView);
};

