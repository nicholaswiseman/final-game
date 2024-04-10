#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

#include "Vec2.h"
#include "eAsset.h"

class Animation
{
	sf::Sprite m_sprite;
	size_t m_numSprites; //total number of animation frames
	size_t m_currentSpriteIdx; //current displayed sprite of the animation loop
	size_t m_frameCount; //frame counter
	size_t m_spriteDur; //the frame duration of each sprite
	Vec2 m_size; //size of animation
	size_t m_lifeTimeFrames; //destroy animation after this many frames
	eAsset m_name;
	std::vector<sf::IntRect> m_textRects;

public:
	Animation();
	Animation(eAsset name, const sf::Texture& t);
	Animation(eAsset name, const sf::Texture& t, size_t frameCount, size_t speed, size_t lifetime, std::vector<sf::IntRect> textRects);

	void Update();
	bool IsEnded() const;
	eAsset GetName() const;
	const Vec2& GetSize() const;
	sf::Sprite& GetSprite();
};

