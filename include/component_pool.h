#pragma once
#include <vector>
#include <unordered_map>
#include <cstdint>

using Entity = uint32_t;

//component pools are sets of a specific component type instance that map to a unique entity

template<typename T>
class ComponentPool {

	std::vector<T> components; //component instances
	std::vector<Entity> entities; //entity id's with an instance of this component type
	std::unordered_map<Entity, size_t> entity_to_index; //maps entity id -> component instance index in components vector

public:

	//adds a component instance of this type to an entity (id)
	void Add(Entity entity, const T& component) { 
		if (entity_to_index.find(entity) != entity_to_index.end()) {
			components[entity_to_index[entity]] = component;
			return;
		}
		size_t index = components.size();
		components.push_back(component);
		entities.push_back(entity);
		entity_to_index[entity] = index;
	}

	//gets component reference from entity id
	T& Get(Entity entity) {
		return components[entity_to_index[entity]];
	}

	//swap-delete when removing component instance
	void Remove(Entity entity) {
		size_t index = entity_to_index[entity];
		entity_to_index.erase(entity);

		components[index] = components.back();
		entities[index] = entities.back();
		entity_to_index[entities[index]] = index;

		components.pop_back();
		entities.pop_back();
	}

	//check if entity has instance of component type
	bool Has(Entity entity) const {
		return entity_to_index.find(entity) != entity_to_index.end();
	}

	//get all instances of component type
	std::vector<T>& GetAllComponents() {
		return components;
	}

	//get all entities with instance of component type
	std::vector<Entity>& GetAllEntities() {
		return entities;
	}
};