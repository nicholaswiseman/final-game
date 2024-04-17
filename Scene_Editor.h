#pragma once
#include "Scene.h"

enum class eEditorView
{
    Main,
    New,
    Load,
};

class Scene_Editor : public Scene
{
public:
    Scene_Editor(sf::RenderWindow* pWindow, GameEngine* pGame);

    void Update() override;
    void sDrag();
    void sCamera();
    void sDoAction(Action action) override;
    void NewDoAction(Action action);
    void LoadDoAction(Action action);
    void AddEntity();
    void PauseAction(Action action);
    void SelectPauseMenuOption();
    void MainDoAction(Action action);
    void GrabEntity();
    void ReleaseEntity();
    void HotMenuAction(Action action);
    void SelectHotMenuOption();
    void sRender() override;
    void DrawHud();
    void RenderAssetPreview();
    void sCombat();
    void sTickTimers();
    void sCollision();
    void ResolveCollision(EntityPointer a, EntityPointer b);
    Vec2 DetectOverlap(EntityPointer a, EntityPointer b);
    Vec2 DetectPreviousOverlap(EntityPointer a, EntityPointer b);
    void ResolveAttackCollision(EntityPointer pAttack, EntityPointer pTarget);
    void ResolveEnemyPlayerCollision(EntityPointer pEnemy, EntityPointer pPlayer);
    void RenderBackground();

    void DoSpecial();

    void sMovement();

    void sParallax();

    void UpdatePlayerMovement();

    void KeepEntityInWindow(EntityPointer e);

    void SpawnBullet();

    void SpawnCannonShot();

    void SpawnForcefield();

    Vec2 CalculateBulletSpawnPoint();

    Vec2 CalculateBulletSpeed();

    float ConvertTransformRotationToRads(float rotation);

    void sBackgroundScroll();

    bool IsEntityOffScreen(EntityPointer e);

    void sEnemyBehavior();

    void PatrolBehavior(EntityPointer e);

    void RenderItemMenu();

    void CreateMenus();

    void SaveLevelToFile();

    void LoadLevelFromFile(std::string filename);

    std::string GetAndDeleteNextStringToken(std::string& str);

    void RenderLoadMenu();

    void PopulateLoadMenu();

    std::vector<std::string> getFilenamesInDirectory(const std::string& directoryPath);

    void DestroyAllEntities();

    sf::Text CreateMenuItem(const sf::Font& font, const std::string& text, float x, float y);

    void Initialize();

    void CreateBackground();

    void RegisterSceneActions();

    void SpawnPlayer();

    void SpawnEnemy(eAsset asset, eBehavior behave, Vec2 pos);

private:
    std::string m_levelPath;
    bool m_drawTextures = true;
    bool m_drawCollision = false;
    eSpecial m_currentSpecial = eSpecial::Forcefield;
    const std::map<eSpecial, std::string> specialHudMap =
    {
        {eSpecial::ElectronCannon, "Electron Cannon"},
        {eSpecial::Forcefield, "Force Field"},
        {eSpecial::Disabled, "Disabled"},
    };

    std::vector<eAsset> m_editorAssets =
    {
        eAsset::Asteroid,
        eAsset::Ufo,
    };
    int m_editorAssetsIdx = 0;

    eEditorView m_currentView;
    std::vector<sf::Text> m_hotMenuItems;
    std::vector<sf::Text> m_pauseMenuItems;
    std::vector<sf::Text> m_loadMenuItems;
    bool m_isPauseMenuActive = false;
    bool m_isHotMenuActive = false;
    bool m_isLoadMenuActive = false;
    int m_selectedPauseIndex = 0;
    int m_selectedHotMenuIndex = 0;
    int m_selectedLoadMenuIndex = 0;
    int m_screenScrollSpeed = 0;
    std::string m_currentFile = "new";
};

