#pragma once

#include <vector>

#include "System.h"

namespace linguine {

class Scene {
  public:
    explicit Scene(std::unique_ptr<EntityManager> entityManager)
        : _entityManager(std::move(entityManager)) {}

    virtual ~Scene() = default;

    void update(float deltaTime) {
      for (const auto& system : _systems) {
        system->update(deltaTime);
      }
    }

    void fixedUpdate(float fixedDeltaTime) {
      for (const auto& system : _systems) {
        system->fixedUpdate(fixedDeltaTime);
      }
    }

  protected:
    inline EntityManager& getEntityManager() {
      return *_entityManager;
    }

    inline std::shared_ptr<Entity> createEntity() {
      return _entityManager->create();
    }

    inline void registerSystem(std::unique_ptr<System> system) {
      _systems.push_back(std::move(system));
    }

  private:
    std::unique_ptr<EntityManager> _entityManager;
    std::vector<std::unique_ptr<System>> _systems;
};

}  // namespace linguine
