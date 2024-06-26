#include "GameEngine.h"
#include "Scene.h"
#include "Scene_StartMenu.h"
#include "Scene_Play.h"
#include "Scene_Game.h"
#include "Scene_Editor.h"

#include <iostream>
#include <map>

GameEngine::GameEngine(const std::string& assetPath)
{
    Initialize(assetPath);
}

void GameEngine::Initialize(const std::string& assetPath)
{
    m_isRunning = true;

    m_assets.LoadAssets();

    m_window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Game");
    m_window.setFramerateLimit(FRAMERATE);
    m_window.setKeyRepeatEnabled(false);
    sf::View view2(sf::FloatRect(0,0,WINDOW_WIDTH, WINDOW_HEIGHT));
    m_window.setView(view2);
    ChangeScene(eScene::Menu, std::make_shared<Scene_StartMenu>(&m_window, this), false);
    //ChangeScene(eScene::Play, std::make_shared<Scene_Play>("assets/level1.txt", &m_window, this), false);
}

void GameEngine::Run()
{
    while (IsRunning())
    {
        sUserInput();
        UpdateScene();
    }
}

void GameEngine::UpdateScene()
{
    m_scenes[m_currentScene]->Update();
}

void GameEngine::Quit()
{
    m_isRunning = false;
}

void GameEngine::ChangeScene(eScene newScene, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
    m_currentScene = newScene;
    m_scenes[m_currentScene] = scene;
}

Assets& GameEngine::GetAssets()
{
    return m_assets;
}

std::shared_ptr<Scene> GameEngine::getCurrentScene()
{
    return m_scenes[m_currentScene];
}

const sf::RenderWindow& GameEngine::GetWindow() const
{
    return m_window;
}

bool GameEngine::IsRunning()
{
    return m_isRunning & m_window.isOpen();
}

void GameEngine::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            Quit();
        }
        
        // Mouse button pressed
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                getCurrentScene()->sDoAction(Action(eAction::LClick, eActionType::Start));
            }
            if (event.mouseButton.button == sf::Mouse::Right) {
                getCurrentScene()->sDoAction(Action(eAction::RClick, eActionType::Start));
            }
            if (event.mouseButton.button == sf::Mouse::Middle)
            {
            }
        }

        // Mouse button released
        if (event.type == sf::Event::MouseButtonReleased)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                getCurrentScene()->sDoAction(Action(eAction::LClick, eActionType::Stop));
            }
            if (event.mouseButton.button == sf::Mouse::Right)
            {
                getCurrentScene()->sDoAction(Action(eAction::RClick, eActionType::Stop));
            }
            if (event.mouseButton.button == sf::Mouse::Middle)
            {
            }
        }

        // Mouse wheel scrolled
        if (event.type == sf::Event::MouseWheelScrolled)
        {
            if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
            {
                if (event.mouseWheelScroll.delta > 0)
                {
                    getCurrentScene()->sDoAction(Action(eAction::WheelUp, eActionType::Start));
                }
                else if (event.mouseWheelScroll.delta < 0)
                {
                    getCurrentScene()->sDoAction(Action(eAction::WheelDown, eActionType::Start));
                }
            }
        }

        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
        {
            ActionMap actMap = getCurrentScene()->GetActionMap();
            if (actMap.find(event.key.code) == actMap.end()) { continue; }

            const eActionType actionType = (event.type == sf::Event::KeyPressed) ? eActionType::Start : eActionType::Stop;
            getCurrentScene()->sDoAction(Action(actMap.at(event.key.code), actionType));
        }
    }
}

void GameEngine::UpdateView(sf::View newView)
{
    m_window.setView(newView);
}

void GameEngine::Play()
{
    ChangeScene(eScene::Play, std::make_shared<Scene_Game>("assets/level1.txt", &m_window, this), false);
}

void GameEngine::ReturnToMainMenu()
{
    ChangeScene(eScene::Menu, std::make_shared<Scene_StartMenu>(&m_window, this), true);
}

void GameEngine::LaunchLevelEditor()
{
    ChangeScene(eScene::Editor, std::make_shared<Scene_Editor>(&m_window, this), true);
}