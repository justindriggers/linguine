#pragma once

#include <vector>

#include "System.h"

#include "ServiceLocator.h"
#import "entity/EntityManagerFactory.h"

namespace linguine {

class Scene {
  public:
    explicit Scene(ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator),
          _entityManager(serviceLocator.get<EntityManagerFactory>().create()) {}

    virtual ~Scene() = default;

    virtual void init() = 0;

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
    template<typename T> inline T& get() {
      return _serviceLocator.get<T>();
    }

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
    ServiceLocator& _serviceLocator;
    std::unique_ptr<EntityManager> _entityManager;
    std::vector<std::unique_ptr<System>> _systems;
};

}  // namespace linguine
