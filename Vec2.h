#pragma once
#include "SFML/System.hpp"

class Vec2
{
public:

	Vec2();
	Vec2(float xin, float yin);

	float x;
	float y;

	bool operator == (const Vec2 &rhs) const;
	bool operator != (const Vec2& rhs) const;
	bool operator >  (const float val) const;
	bool operator <  (const float val) const;
	bool operator >=  (const float val) const;
	bool operator <=  (const float val) const;

	Vec2 operator + (const Vec2& rhs) const;
	Vec2 operator - (const Vec2& rhs) const;
	Vec2 operator * (const float val) const;
	Vec2 operator / (const float val) const;

	void operator += (const Vec2& rhs);
	void operator -= (const Vec2& rhs);
	void operator *= (const float val);
	void operator /= (const float val);

	Vec2 Normalize(const float size);
	float Absolute();
	sf::Vector2f ToSFMLVec2() const;

	float Cross(Vec2 v);

};