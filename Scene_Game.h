#pragma once
#include "Scene.h"
class Scene_Game : public Scene
{
public:
    Scene_Game(const std::string& levelPath, sf::RenderWindow* pWindow, GameEngine* pGame);

    void Update() override;
    void sDoAction() override;
    void sRender() override;
    void DoAction(Action action) override;

    void sMovement();

    void SpawnBullet();

    Vec2 CalculateBulletSpawnPoint();

    Vec2 CalculateBulletSpeed();

    float ConvertTransformRotationToRads(float rotation);

    void Initialize(const std::string& levelPath);

    void RegisterSceneActions();

    void SpawnPlayer();

private:
    std::string m_levelPath;
    bool m_drawTextures = true;
    bool m_drawCollision = false;
};

