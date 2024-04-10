#include "Animation.h"

Animation::Animation()
{
}

Animation::Animation(eAsset name, const sf::Texture& t)
{
}

Animation::Animation(eAsset name, const sf::Texture& t, size_t frameCount, size_t speed, size_t lifetime, std::vector<sf::IntRect> textRects)
    : m_name (name)
    , m_sprite (t)
    , m_numSprites (frameCount)
    , m_frameCount (0)
    , m_currentSpriteIdx (0)
    , m_spriteDur (speed)
    , m_lifeTimeFrames (lifetime)
    , m_textRects (textRects)
{
    m_size = Vec2((float)textRects[0].getSize().x, (float)textRects[0].getSize().y);
    m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
    m_sprite.setTextureRect(m_textRects[0]);
}

void Animation::Update()
{
    m_frameCount++;
    if (m_numSprites <= 1) { return; }
    //add the speed varriable to the current frame

    //TODO: 1 calculate the correct frame of animation to play based on currentFrame and speed
     //     2 set the texted rectangle properly

    if (m_frameCount % m_spriteDur == 0)
    {
        m_currentSpriteIdx++;
        m_sprite.setTextureRect(m_textRects[m_currentSpriteIdx % m_numSprites]);
    }
}

bool Animation::IsEnded() const
{
    if (m_lifeTimeFrames == 0) { return false; }
    return (m_frameCount >= m_lifeTimeFrames);
}

eAsset Animation::GetName() const
{
    return m_name;
}

const Vec2& Animation::GetSize() const
{
    return m_size;
}

sf::Sprite& Animation::GetSprite()
{
    return m_sprite;
}
