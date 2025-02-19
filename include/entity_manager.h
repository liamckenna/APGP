#pragma once
#include <cstdint>
#include <vector>
#include <algorithm>

using Entity = uint32_t;

//entity manager maintains all active entities in the scene

class EntityManager {
	uint32_t next_entity_id = 0;
	std::vector<Entity> entities;

public:

	Entity CreateEntity() {
		Entity id = next_entity_id++;
		entities.push_back(id);
		return id;
	}

	void DestroyEntity(Entity entity) {
		entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
	}

	const std::vector<Entity>& GetEntities() const {
		return entities;
	}
};