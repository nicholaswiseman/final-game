#include "Vec2.h"
#include <math.h>

Vec2::Vec2()
{
	x = 0;
	y = 0;
}

Vec2::Vec2(float xin, float yin)
	: x(xin), y(yin)
{

}

bool Vec2::operator == (const Vec2& rhs) const
{
	return ((x == rhs.x) && (y == rhs.y));
}

bool Vec2::operator != (const Vec2& rhs) const
{
	return !((x == rhs.x) && (y == rhs.y));
}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (const float val) const
{
	return Vec2(x * val, y * val);
}

Vec2 Vec2::operator / (const float val) const
{
	return Vec2(x / val, y / val);
}

void Vec2::operator += (const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

void Vec2::operator *= (const float val)
{
	x *= val;
	y *= val;
}

void Vec2::operator /= (const float val)
{
	x /= val;
	y /= val;
}

bool Vec2::operator > (const float val) const
{
	return ((x * x + y * y) > val);
}

bool Vec2::operator < (const float val) const
{
	return ((x * x + y * y) < val);
}

bool Vec2::operator >= (const float val) const
{
	return ((x * x + y * y) >= val);
}

bool Vec2::operator <= (const float val) const
{
	return ((x * x + y * y) <= val);
}

float Vec2::Absolute()
{
	return sqrt((x * x) + (y * y));
}

Vec2 Vec2::Normalize(const float size)
{
	float angle = atan2f(y, x);
	x = size * cos(angle);
	y = size * sin(angle);
	return Vec2(x, y);
}

sf::Vector2f Vec2::ToSFMLVec2() const
{
	return sf::Vector2f(x, y);
}

float Vec2::Cross(Vec2 v)
{
	return (x * v.y) - (y * v.x);
}