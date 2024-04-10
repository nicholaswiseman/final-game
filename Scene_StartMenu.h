#pragma once
#include "Scene.h"
class Scene_StartMenu : public Scene
{
public:
    void Update() override;
    void sDoAction() override;
    void DoAction(Action action) override;
    void OptionSelection();
    void sRender() override;
    void Initialize();
    void RegisterActions();
    Scene_StartMenu(sf::RenderWindow* pWindow, GameEngine* pGame);

private:
    int m_selectedOptionIndex;
    std::vector<sf::Text> m_menuOptions;
};

