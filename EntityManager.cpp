#include "EntityManager.h"

void EntityManager::Update()
{
	AddNewEntities();
	RemoveInactiveEntities();
}

void EntityManager::AddNewEntities()
{
	for (EntityPointer e : m_toAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->getTag()].push_back(e);
	}
	m_toAdd.clear();
}

void EntityManager::RemoveInactiveEntities()
{
	RemoveInactiveEntitiesFromList(m_entities);

	for (auto& taggedList : m_entityMap)
	{
		RemoveInactiveEntitiesFromList(taggedList.second);
	}

}

void EntityManager::RemoveInactiveEntitiesFromList(EntityList& list)
{
	auto entityIterator = list.begin();
	while (entityIterator != list.end())
	{
		if ((*entityIterator)->isInactive())
		{
			entityIterator = list.erase(entityIterator);
		}
		else
		{
			entityIterator++;
		}
	}
}

std::shared_ptr<Entity> EntityManager::addEntity(eEntityTag tag)
{
	EntityPointer pEntity = std::shared_ptr<Entity>(new Entity(tag, m_totalEntities++));

	m_toAdd.push_back(pEntity);

	return pEntity;
}

std::shared_ptr<Entity> EntityManager::getEntity(size_t idToGet)
{
	return std::shared_ptr<Entity>();
}

std::shared_ptr<EntityList> EntityManager::getEntities()
{
	return std::make_shared<EntityList>(m_entities);
}

std::shared_ptr<EntityList> EntityManager::getEntities(eEntityTag tagToGet)
{
	return std::make_shared<EntityList>(m_entityMap[tagToGet]);
}

EntityPointer EntityManager::getPlayer()
{
	EntityList playerList = *getEntities(eEntityTag::Player);
	EntityPointer player = nullptr;

	if (playerList.size() > 0)
	{
		player = playerList[0];
	}
	return player;
}