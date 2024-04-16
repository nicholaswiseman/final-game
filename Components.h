#pragma once
#include "Vec2.h"
#include "SFML/Graphics.hpp"
#include "Animation.h"
#include "ePlayerState.h"

enum class eBehavior
{
	Patrol,
	Chase,
	Idle,
};

class Component
{
public:
	bool has = false;
};

class CTransform : public Component
{
public:
	Vec2 pos;
	Vec2 prevPos;
	Vec2 velocity;
	Vec2 scale = Vec2(1,1);
	float angle;
};

class CInput : public Component
{
public:
	bool jump;
	bool up;
	bool down;
	bool left;
	bool right;
	bool leftClick;
	bool rightClick;
	Vec2 mousePos;
	bool canAttack;
};

class CCollision : public Component
{

};

class CBoundingBox : public Component
{
public:
	CBoundingBox()
	{
		has = true;
	}
	CBoundingBox(sf::RectangleShape boundBox)
	{
		box = boundBox;
		has = true;
	}
	sf::RectangleShape box;
};

class CAnimation : public Component
{
public:
	CAnimation()
	{
		has = true;
	}
	CAnimation(Animation ani)
	{
		animation = ani;
		has = true;
	}
	Animation animation;
};

class CHealth : public Component
{
public:
	CHealth()
	{
		has = true;
	}
	CHealth(int hp)
	{
		health = hp;
		has = true;
	}
	int health;
};

class CDamage : public Component
{
public:
	CDamage()
	{
		has = true;
	}
	CDamage(int dmg)
	{
		damage = dmg;
		has = true;
	}
	int damage;
};

class CBehavior : public Component
{
public:
	CBehavior()
	{
		has = true;
	}
	CBehavior(eBehavior behave, std::vector<Vec2> posList)
	{
		behavior = behave;
		positions = posList;
		posIdx = 0;
	}
	eBehavior behavior;
	std::vector<Vec2> positions;
	int posIdx;
};

class CBlockVision : public Component
{
public:
	bool blocksVision;
};

class CInvincibility : public Component
{
public:
	CInvincibility()
	{
		has = true;
		duration = 0;
	}
	CInvincibility(int frameDuration)
	{
		duration = frameDuration;
		has = true;
	}
	int duration;
};

class CParallax : public Component
{
public:
	CParallax()
	{
		has = true;
		factor = 1;
	}
	CParallax(float fact)
	{
		factor = fact;
		has = true;
	}
	float factor;
};

class CLifetime : public Component
{
public:
	CLifetime()
	{

	}
	CLifetime(int life)
	{
		lifetimeFrames = life;
		has = true;
	}
	int lifetimeFrames;
};

class CAttackCoolDown : public Component
{
public:
	CAttackCoolDown()
	{

	}
	CAttackCoolDown(int time)
	{
		cooldown = time;
		has = true;
	}
	int cooldown;
};

class CSpecialCoolDown : public Component
{
public:
	CSpecialCoolDown()
	{

	}
	CSpecialCoolDown(int time)
	{
		cooldown = time;
		has = true;
	}
	int cooldown;
};


class CDraggable : public Component
{
public:
	CDraggable()
	{

	}
	bool dragging = false;
};