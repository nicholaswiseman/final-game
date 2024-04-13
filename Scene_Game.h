#pragma once
#include "Scene.h"

class Scene_Game : public Scene
{
public:
    Scene_Game(const std::string& levelPath, sf::RenderWindow* pWindow, GameEngine* pGame);

    void Update() override;
    void sCamera();
    void sDoAction() override;
    void sRender() override;
    void DrawHud();
    void sCombat();
    void sTickTimers();
    void sCollision();
    void ResolveCollision(EntityPointer a, EntityPointer b);
    Vec2 DetectOverlap(EntityPointer a, EntityPointer b);
    Vec2 DetectPreviousOverlap(EntityPointer a, EntityPointer b);
    void ResolveAttackCollision(EntityPointer pAttack, EntityPointer pTarget);
    void ResolveEnemyPlayerCollision(EntityPointer pEnemy, EntityPointer pPlayer);
    void RenderBackground();
    void DoAction(Action action) override;

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

    void Initialize(const std::string& levelPath);

    void CreateBackground();

    void RegisterSceneActions();

    void SpawnPlayer();

    void SpawnEnemy();

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
};

