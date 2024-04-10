#include "Assets.h"
#include "eAsset.h"
#include "SFML/Graphics.hpp"
#include "Config.h"

#include <iostream>


void Assets::LoadAssets()
{
    //load all textures
    for (auto sheet : textureConfig)
    {
        LoadTextureSheet(sheet);
    }

    for (auto animation : animationConfig)
    {
        LoadAnimation(animation);
    }

    AddFont(eAsset::SerifFont, "assets\\fonts\\liberation-serif\\LiberationSerif-Regular.ttf");
}

void Assets::AddTexture(eTexture name, std::string path)
{
    if (!m_textures[name].loadFromFile(path))
    {
        std::cout << "FAILED TO LOAD: " << path << std::endl;
    }
}

void Assets::AddAnimation(eAsset name, Animation animation)
{
    m_animations[name] = animation;
}

void Assets::AddSound(eAsset name, std::string path)
{
}

void Assets::AddFont(eAsset name, std::string path)
{
    if (!m_fonts[name].loadFromFile(path))
    {
        std::cerr << "Failed to load font!" << std::endl;
        exit(-1);
    }
}

sf::Texture& Assets::GetTexture(eTexture name)
{
    return m_textures[name];
}

Animation& Assets::GetAnimation(eAsset name)
{
    return m_animations[name];
}

sf::Sound& Assets::GetSound(eAsset name)
{
    return m_sounds[name];
}

sf::Font& Assets::GetFont(eAsset name)
{
    return m_fonts[name];
}

void Assets::LoadTextureSheet(std::tuple<eTexture, std::string> textToLoad)
{
    AddTexture(std::get<0>(textToLoad), std::get<1>(textToLoad));
}

void Assets::LoadAnimation(std::tuple<aniName, texture, numFrames, frameDuration, aniLifetime, TextRectList> animationConfig)
{
    //eAsset name, const sf::Texture& t, size_t frameCount, size_t speed, size_t animationLifetime std::vector<sf::IntRect> textRects
    eAsset name = std::get<0>(animationConfig);
    const sf::Texture t = m_textures[std::get<1>(animationConfig)];
    size_t frameCount = std::get<2>(animationConfig);
    size_t frameDur = std::get<3>(animationConfig);
    size_t life = std::get<4>(animationConfig);
    std::vector<sf::IntRect> textureRectanlges = std::get<5>(animationConfig);

    Animation a = Animation(name, m_textures[std::get<1>(animationConfig)], frameCount, frameDur, life, textureRectanlges);
    AddAnimation(a.GetName(), a);
}