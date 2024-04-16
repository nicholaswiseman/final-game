#include "Scene_StartMenu.h"


#include <iostream>
Scene_StartMenu::Scene_StartMenu(sf::RenderWindow* pWindow, GameEngine* pGame)
	: Scene(pWindow, pGame)
{
	Initialize();
}

void Scene_StartMenu::Update()
{
    sRender();
}

void Scene_StartMenu::sDoAction(Action action)
{
    DoAction(action);
}

void Scene_StartMenu::DoAction(Action action)
{
    if (action.GetType() == eActionType::Start)
    {
        switch (action.GetName())
        {
        case eAction::CursorUp:
            m_selectedOptionIndex = (m_selectedOptionIndex - 1 + m_menuOptions.size()) % m_menuOptions.size();
            break;
        case eAction::CursorDown:
            m_selectedOptionIndex = (m_selectedOptionIndex + 1) % m_menuOptions.size();
            break;
        case eAction::SelectOption:
            OptionSelection();
            break;
        case eAction::ActionCount:
            std::cout << "ActionCount (not a valid action)\n";
            break;
        }
    }
}

void Scene_StartMenu::OptionSelection()
{
    if (m_menuOptions[m_selectedOptionIndex].getString() == "Exit")
    {
        m_pWindow->close(); // Close the window if "Exit" is selected
    }
    else if (m_menuOptions[m_selectedOptionIndex].getString() == "New Game")
    {
        m_pGame->Play();
    }
    else if (m_menuOptions[m_selectedOptionIndex].getString() == "Level Editor")
    {
        m_pGame->LaunchLevelEditor();
    }

}

void Scene_StartMenu::sRender()
{
    m_pWindow->clear();

    // Draw title text
    sf::Text titleText("Main Menu", m_pGame->GetAssets().GetFont(eAsset::SerifFont), 48);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(300.f, 50.f);
    m_pWindow->draw(titleText);

    // Draw menu options, highlighting the selected option
    for (size_t i = 0; i < m_menuOptions.size(); ++i)
    {
        if (i == m_selectedOptionIndex)
        {
            m_menuOptions[i].setFillColor(sf::Color::Yellow); // Highlight selected option
        }
        else {
            m_menuOptions[i].setFillColor(sf::Color::White);
        }
        m_pWindow->draw(m_menuOptions[i]);
    }

    m_pWindow->display();
}

void Scene_StartMenu::Initialize()
{
    RegisterActions();

    std::vector<std::string> optionStrings = { "New Game", "Load Game", "Options", "Level Editor", "Exit" };

    for (size_t i = 0; i < optionStrings.size(); ++i)
    {
        sf::Text optionText(optionStrings[i], m_pGame->GetAssets().GetFont(eAsset::SerifFont), 24);
        optionText.setFillColor(sf::Color::White);
        optionText.setPosition(300.f, 150.f + i * 50.f);
        m_menuOptions.push_back(optionText);
    }

    // Index of the currently selected menu option
    m_selectedOptionIndex = 0;
}

void Scene_StartMenu::RegisterActions()
{
    RegisterAction(sf::Keyboard::Escape, eAction::Quit);
    RegisterAction(sf::Keyboard::W, eAction::CursorUp);
    RegisterAction(sf::Keyboard::S, eAction::CursorDown);
    RegisterAction(sf::Keyboard::Up, eAction::CursorUp);
    RegisterAction(sf::Keyboard::Down, eAction::CursorDown);
    RegisterAction(sf::Keyboard::Space, eAction::SelectOption);
    RegisterAction(sf::Keyboard::Enter, eAction::SelectOption);
}