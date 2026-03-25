#include "ECS.h"

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
	// Component arrays are managed by shared_ptr, automatic cleanup
}

EntityID EntityManager::createEntity()
{
	EntityID entity;
	
	if (!availableIDs.empty())
	{
		entity = availableIDs.back();
		availableIDs.pop_back();
	}
	else
	{
		entity = nextEntityID++;
	}
	
	validEntities[entity] = true;
	return entity;
}

void EntityManager::destroyEntity(EntityID entity)
{
	if (!isValid(entity))
	{
		return;
	}
	
	// Remove all components for this entity
	for (auto& pair : componentArrays)
	{
		pair.second->entityDestroyed(entity);
	}
	
	validEntities.erase(entity);
	availableIDs.push_back(entity);
}

bool EntityManager::isValid(EntityID entity) const
{
	if (entity == NULL_ENTITY)
	{
		return false;
	}
	
	auto it = validEntities.find(entity);
	return it != validEntities.end() && it->second;
}
