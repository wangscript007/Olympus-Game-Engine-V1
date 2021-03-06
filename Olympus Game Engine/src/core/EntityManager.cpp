#include "EntityManager.h"

EntityManager::EntityManager() {

}
void EntityManager::addEntity(Entity* entity) {
	m_entityList.push_back(entity);
}

Entity* EntityManager::getEntity(int ID) {
	for (auto entity : m_entityList) {
		if (entity->m_ID == ID)
			return entity;
	}
}

void EntityManager::initPhysics() {
	for (auto entity : m_entityList) {
		if (entity->GetComponent<PhysicsComponent>() != nullptr) {
			entity->update();
		}
	}
}

void EntityManager::render() {
	for (auto entity : m_entityList) {
		if (entity->GetComponent<CubeGraphicsComponent>() != nullptr || entity->GetComponent<PlaneGraphicsComponent>() != nullptr || entity->GetComponent<ModelComponent>() != nullptr || entity->GetComponent<BillboardComponent>() != nullptr||  entity->GetComponent<ParticleGenerator>() != nullptr || entity->GetComponent<SphereGraphicsComponent>() != nullptr) {
			entity->update();
		}
	}
}

void EntityManager::renderShadow() {
	for (auto entity : m_entityList) {
		if (entity->GetComponent<CubeGraphicsComponent>() != nullptr || entity->GetComponent<ModelComponent>() != nullptr || entity->GetComponent<SphereGraphicsComponent>() != nullptr || entity->GetComponent<SphereGraphicsComponent>() != nullptr) {
			entity->updateShadow();
		}
	}
}

void EntityManager::renderLight() {
	for (auto entity : m_entityList) {
		if (entity->GetComponent<DirectionalLightComponent>() != nullptr || entity->GetComponent<LightComponent>() != nullptr) {
			entity->update();
		}
	}
}
std::vector<Entity*> EntityManager::getEntityList() {
	return m_entityList;
}

