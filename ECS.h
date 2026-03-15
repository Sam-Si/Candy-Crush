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
	Typed component storage using sparse set for cache-friendly iteration.
	Sparse set uses two vectors:
	- dense: contiguous storage of (EntityID, T) pairs for cache locality
	- sparse: maps EntityID to index in dense vector (or -1 if not present)
*/
template<typename T>
class ComponentArray : public IComponentArray
{
public:
	void insert(EntityID entity, T component)
	{
		// Ensure sparse array is large enough
		if (entity >= sparse.size())
		{
			sparse.resize(entity + 1, -1);
		}

		// If entity already has this component, update it
		if (sparse[entity] != -1)
		{
			dense[sparse[entity]].second = component;
			return;
		}

		// Add to dense array
		sparse[entity] = static_cast<int>(dense.size());
		dense.emplace_back(entity, std::move(component));
	}

	void remove(EntityID entity)
	{
		if (entity >= sparse.size() || sparse[entity] == -1)
		{
			return;
		}

		// Get index of element to remove
		int index = sparse[entity];
		int lastIndex = static_cast<int>(dense.size()) - 1;

		// If not the last element, swap with last
		if (index != lastIndex)
		{
			EntityID lastEntity = dense[lastIndex].first;
			dense[index] = std::move(dense[lastIndex]);
			sparse[lastEntity] = index;
		}

		// Remove last element and mark sparse as empty
		dense.pop_back();
		sparse[entity] = -1;
	}

	T* get(EntityID entity)
	{
		if (entity >= sparse.size() || sparse[entity] == -1)
		{
			return nullptr;
		}
		return &dense[sparse[entity]].second;
	}

	bool has(EntityID entity) const
	{
		return entity < sparse.size() && sparse[entity] != -1;
	}

	void entityDestroyed(EntityID entity) override
	{
		remove(entity);
	}

	// Returns contiguous dense storage for cache-friendly iteration
	const std::vector<std::pair<EntityID, T>>& getAll() const
	{
		return dense;
	}

	std::size_t size() const
	{
		return dense.size();
	}

private:
	// Dense array: contiguous storage for cache locality
	// Stores (EntityID, Component) pairs
	std::vector<std::pair<EntityID, T>> dense;

	// Sparse array: maps EntityID to index in dense array (-1 = not present)
	std::vector<int> sparse;
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
	const std::vector<std::pair<EntityID, T>>& getAllComponents()
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
