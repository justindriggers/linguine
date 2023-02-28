#pragma once

#include "entity/Entity.h"
#include "entity/EntityManager.h"
#include "entity/Result.h"

namespace linguine {

class System {
  public:
    explicit System(EntityManager& entityManager)
        : _entityManager(entityManager) {}

    virtual ~System() = default;

    virtual void update(float deltaTime) = 0;

    virtual void fixedUpdate(float fixedDeltaTime) = 0;

  protected:
    inline std::shared_ptr<Entity> createEntity() {
      return _entityManager.create();
    }

    inline std::shared_ptr<Entity> getEntityById(uint64_t id) {
      return _entityManager.getById(id);
    }

    template<typename... Types>
    inline std::shared_ptr<Result> findEntities() {
      return _entityManager.find<Types...>();
    }

  private:
    EntityManager& _entityManager;
};

}  // namespace linguine
