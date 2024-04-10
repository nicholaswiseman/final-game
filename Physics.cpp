#include "Physics.h"
#include "Components.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	//TODO return the overlap rectangle size of the BB of a and b
	return Vec2(0, 0);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	//TODO: return the previous overlap rectangle size of the BB a and b
	return Vec2(0, 0);
}
