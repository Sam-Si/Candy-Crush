#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <memory>
#include <type_traits>

/*
	Entity Component System (ECS) foundation.
	EntityID is a simple integer identifier.
	EntityManager owns all component data and maps entities to components.
*/

using EntityID = uint32_t;
const EntityID NULL_ENTITY = 0;

/*
	Component storage base class for type erasure.
*/
class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void entityDestroyed(EntityID entity) = 0;
};

/*
	Typed component storage using unordered_map for sparse storage.
*/
template<typename T>
class ComponentArray : public IComponentArray
{
public:
	void insert(EntityID entity, T component)
	{
		components[entity] = component;
	}

	void remove(EntityID entity)
	{
		components.erase(entity);
	}

	T* get(EntityID entity)
	{
		auto it = components.find(entity);
		if (it != components.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	bool has(EntityID entity) const
	{
		return components.find(entity) != components.end();
	}

	void entityDestroyed(EntityID entity) override
	{
		remove(entity);
	}

	std::unordered_map<EntityID, T>& getAll()
	{
		return components;
	}

private:
	std::unordered_map<EntityID, T> components;
};

/*
	EntityManager creates entities and manages component storage.
	Central registry for all entities and their components.
*/
class EntityManager
{
public:
	EntityManager();
	~EntityManager();

	// Entity lifecycle
	EntityID createEntity();
	void destroyEntity(EntityID entity);
	bool isValid(EntityID entity) const;

	// Component management
	template<typename T>
	void addComponent(EntityID entity, T component)
	{
		getComponentArray<T>()->insert(entity, component);
	}

	template<typename T>
	void removeComponent(EntityID entity)
	{
		getComponentArray<T>()->remove(entity);
	}

	template<typename T>
	T* getComponent(EntityID entity)
	{
		return getComponentArray<T>()->get(entity);
	}

	template<typename T>
	const T* getComponent(EntityID entity) const
	{
		return getComponentArray<T>()->get(entity);
	}

	template<typename T>
	bool hasComponent(EntityID entity) const
	{
		return getComponentArray<T>()->has(entity);
	}

	template<typename T>
	std::unordered_map<EntityID, T>& getAllComponents()
	{
		return getComponentArray<T>()->getAll();
	}

private:
	EntityID nextEntityID = 1;
	std::vector<EntityID> availableIDs;
	std::unordered_map<EntityID, bool> validEntities;

	// Component storage
	std::unordered_map<std::size_t, std::shared_ptr<IComponentArray>> componentArrays;

	template<typename T>
	ComponentArray<T>* getComponentArray()
	{
		std::size_t typeHash = typeid(T).hash_code();
		
		if (componentArrays.find(typeHash) == componentArrays.end())
		{
			componentArrays[typeHash] = std::make_shared<ComponentArray<T>>();
		}
		
		return static_cast<ComponentArray<T>*>(componentArrays[typeHash].get());
	}

	template<typename T>
	ComponentArray<T>* getComponentArray() const
	{
		std::size_t typeHash = typeid(T).hash_code();
		
		auto it = componentArrays.find(typeHash);
		if (it != componentArrays.end())
		{
			return static_cast<ComponentArray<T>*>(it->second.get());
		}
		return nullptr;
	}
};
