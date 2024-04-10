#include "Entity.h"

Entity::Entity(eEntityTag tag, size_t totalEntities)
	: m_tag(tag), m_id(totalEntities)
{
	m_active = true;
}

void Entity::markInactive()
{
 	m_active = false;
}

bool Entity::isInactive()
{
	return !m_active;
}

eEntityTag Entity::getTag()
{
	return m_tag;
}

size_t Entity::getId()
{
	return m_id;
}