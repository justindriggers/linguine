#pragma once

#include "System.h"
#include "factories/ProjectileFactory.h"

namespace linguine {

class ProjectileSpawnSystem : public System {
  public:
    ProjectileSpawnSystem(EntityManager& entityManager,
                          ProjectileFactory& projectileFactory)
        : System(entityManager), _projectileFactory(projectileFactory) {}

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    ProjectileFactory& _projectileFactory;
};

}  // namespace linguine
