#pragma once

#include "Entity.h"

#include <vector>
#include <map>
#include <memory>

typedef std::shared_ptr<Entity> EntityPointer;
typedef  std::vector<EntityPointer> EntityList;

class EntityManager
{
private:
	EntityList m_entities;
	std::map<eEntityTag, EntityList> m_entityMap;
	EntityList m_toAdd;
	size_t m_totalEntities = 0;

	void Initialize();
	void AddNewEntities();
	void RemoveInactiveEntities();
	void RemoveInactiveEntitiesFromList(EntityList& list);

public:	
	void Update();
	EntityPointer addEntity(eEntityTag tag);
	EntityPointer getEntity(size_t idToGet);
	std::shared_ptr<EntityList> getEntities();
	std::shared_ptr<EntityList> getEntities(eEntityTag tagToGet);
	EntityPointer getPlayer();
};

