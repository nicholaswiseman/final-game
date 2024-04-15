#pragma once
#include "Scene.h"

class Scene_Play : public Scene
{
private:
    std::map<std::string, eAsset> m_charToTileMap
    {
        {"GreenMountainTL", eAsset::GreenMountainTL},
        {"GreenMountainTM", eAsset::GreenMountainTM},
        {"GreenMountainTR", eAsset::GreenMountainTR},
        {"GreenMountainBL", eAsset::GreenMountainBL},
        {"GreenMountainBM", eAsset::GreenMountainBM},
        {"GreenMountainBR", eAsset::GreenMountainBR},
        {"Tree"           , eAsset::Tree           },
    };

    std::map<std::string, eAsset> m_charToEnemyMap
    {
        {"Tektite", eAsset::Tektite},
    };

public:
    std::string m_levelPath;
    bool m_drawTextures = true;
    bool m_drawCollision = false;
    bool m_drawGrid = false;
    bool m_linkCam = false;

    void Update() override;
    void sDoAction(Action action) override;
    void sRender() override;

    void DrawHud();

    void DrawGrid();

    Scene_Play(const std::string& levelPath, sf::RenderWindow* pWindow, GameEngine* pGame);
    void Init(const std::string& levelPath);
    void LoadLevel(const std::string& levelpath);
    void SpawnObject(std::string obj);
    void SpawnTile(std::string obj);
    void SpawnSword();
    Vec2 GridToMidPixel(float roomX, float roomY, float gridX, float gridY, std::shared_ptr<Entity> entity);
    std::string GetAndDeleteNextStringToken(std::string& str);
    std::vector<std::string> ReadLevelFile(std::string levelPath);
    void SpawnPlayer();
    void SpawnMeat();
    virtual void DoAction(Action action) override;
    void sMovement();
    void sCollision();
    void ResolveItemPickUp(EntityPointer pItem);
    void ResolveAttackCollision(EntityPointer pAttack, EntityPointer pTarget);
    void ResolveEnemyPlayerCollision(EntityPointer pEnemy, EntityPointer pPlayer);
    void sCamera();
    void sCombat();
    void UpdateSwordTransform(EntityPointer pSword);
    sf::Vector2i GetRoom(Vec2 pos);
    void sRespawn();
    void sAnimation();
    void SpawnEnemy(std::string enemyConfig);
    void sEnemyBehavior();
    void PatrolBehavior(EntityPointer e);
    void ChaseBehavior(EntityPointer e);
    bool LineIntersect(Vec2 a, Vec2 b, Vec2 c, Vec2 d);
    bool EntityIntersect(EntityPointer e, Vec2 a, Vec2 b);
    bool PathClear(Vec2 a, Vec2 b);
    Vec2 DetectOverlap(EntityPointer a, EntityPointer b);
    Vec2 DetectPreviousOverlap(EntityPointer a, EntityPointer b);
    void ResolveCollision(EntityPointer a, EntityPointer b);
};