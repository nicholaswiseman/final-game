#pragma once
#include <map>

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

#include "eAsset.h"
#include "Animation.h"
#include "Config.h"

class Assets
{
private:
	std::map<eTexture, sf::Texture> m_textures;
	std::map<eAsset, Animation> m_animations;
	std::map<eAsset, sf::Sound> m_sounds;
	std::map<eAsset, sf::Font> m_fonts;

	std::vector<std::tuple<eTexture, std::string>> textureConfig =
	{
		{eTexture::MarioTilesheet, "assets/tileset.png"},
		{eTexture::Link, "assets/link.png"},
		{eTexture::Overworld, "assets/overworld.png"},
		{eTexture::Monsters, "assets/monsters-overworld.png"},
		{eTexture::Ship2D, "assets/space/Ship_2_D_Small.png"},
		{eTexture::Bullet, "assets/space/bullet.png"},
		{eTexture::SpaceBg, "assets/space/space-bg.png"},
		{eTexture::Earth, "assets/space/earf.png"},
		{eTexture::Ufo, "assets/space/ufo.png"},
		{eTexture::Asteroid, "assets/space/asteroid.png"}
	};

	typedef size_t numFrames;
	typedef size_t frameDuration;
	typedef size_t aniLifetime;
	typedef std::vector<sf::IntRect> TextRectList;
	typedef eAsset aniName;
	typedef eTexture texture;
	std::vector<std::tuple
		<aniName, texture, numFrames, frameDuration, aniLifetime, TextRectList>> animationConfig =
	{
		{eAsset::Brick, eTexture::MarioTilesheet, 1, 0, 0,
			{sf::IntRect(17, 16, DEFAULT_TILE_SIZE_X, DEFAULT_TILE_SIZE_Y)}},
		{eAsset::LinkIdleDown, eTexture::Link, 1, 0, 0,
			{sf::IntRect(1, 11, 16, 16)}},
		{eAsset::LinkWalkDown, eTexture::Link, 2, 5, 0,
			{{sf::IntRect(1, 11, 16, 16)},
			{sf::IntRect(18, 11, 16, 16)}}},
		{eAsset::LinkIdleSide, eTexture::Link, 1, 0, 0,
			{sf::IntRect(35, 11, 16, 16)}},
		{eAsset::LinkWalkSide, eTexture::Link, 2, 5, 0,
			{{sf::IntRect(35, 11, 16, 16)},
			{sf::IntRect(52, 11, 16, 16)}}},
		{eAsset::LinkIdleUp, eTexture::Link, 1, 0, 0,
			{sf::IntRect(69, 11, 16, 16)}},
		{eAsset::LinkWalkUp, eTexture::Link, 2, 5, 0,
			{{sf::IntRect(69, 11, 16, 16)},
			{sf::IntRect(86, 11, 16, 16)}}},
		{eAsset::GreenMountainTL, eTexture::Overworld, 1, 0, 0,
			{sf::IntRect(1632, 1440, 16, 16)}},
		{eAsset::GreenMountainTM, eTexture::Overworld, 1, 0, 0,
			{sf::IntRect(1648, 1440, 16, 16)}},
		{eAsset::GreenMountainTR, eTexture::Overworld, 1, 0, 0,
			{sf::IntRect(1664, 1440, 16, 16)}},
		{eAsset::GreenMountainBL, eTexture::Overworld, 1, 0, 0,
			{sf::IntRect(1632, 1456, 16, 16)}},
		{eAsset::GreenMountainBM, eTexture::Overworld, 1, 0, 0,
			{sf::IntRect(1648, 1456, 16, 16)}},
		{eAsset::GreenMountainBR, eTexture::Overworld, 1, 0, 0,
			{sf::IntRect(1664, 1456, 16, 16)}},
		{eAsset::Tree, eTexture::Overworld, 1, 0, 0,
			{sf::IntRect(1600, 1440, 16, 16)}},
		{eAsset::Sword, eTexture::Link, 1, 0, 10,
			{sf::IntRect(1, 154, 7, 16)}},
		{eAsset::Tektite, eTexture::Monsters, 2, 10, 0,
			{{sf::IntRect(162, 90, 16, 16)},
			{sf::IntRect(179, 90, 16, 16)}}},
		{eAsset::Meat, eTexture::Link, 1, 0, 0,
			{sf::IntRect(299, 185, 8, 16)}},
		{eAsset::PlayerShip, eTexture::Ship2D, 1, 0, 0,
			{sf::IntRect(0, 0, 128, 128)}},
		{eAsset::Bullet, eTexture::Bullet, 1, 0, 0,
			{sf::IntRect(0, 0, 8, 32)}},
		{eAsset::SpaceBg, eTexture::SpaceBg, 1, 0, 0,
			{sf::IntRect(0, 0, 768, 896)}},
		{eAsset::Earth, eTexture::Earth, 1, 0, 0,
			{sf::IntRect(0, 0, 64, 64)}},
		{eAsset::Ufo, eTexture::Ufo, 1, 0, 0,
			{sf::IntRect(0, 0, 64, 32)}},
		{eAsset::Asteroid, eTexture::Asteroid, 1, 0, 0,
			{sf::IntRect(0, 0, 64, 64)}},
	};

	void LoadAnimation(std::tuple<aniName, Assets::texture, Assets::numFrames, Assets::frameDuration, Assets::aniLifetime, Assets::TextRectList> animationConfig);
	void LoadTextureSheet(std::tuple<eTexture, std::string> textToLoad);

	void AddTexture(eTexture name, std::string path);
	void AddAnimation(eAsset name, Animation animation);
	void AddSound(eAsset name, std::string path);
	void AddFont(eAsset name, std::string path);

public:
	void LoadAssets();
	sf::Texture& GetTexture(eTexture name);
	Animation& GetAnimation(eAsset name);
	sf::Sound& GetSound(eAsset name);
	sf::Font& GetFont(eAsset name);
};

