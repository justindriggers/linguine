#pragma once

#include "System.h"

#include "data/Grid.h"
#include "factories/ProjectileFactory.h"

namespace linguine {

class AttackSystem : public System {
  public:
    AttackSystem(EntityManager& entityManager,
                 Grid& grid,
                 ProjectileFactory& projectileFactory)
        : System(entityManager), _grid(grid),
          _projectileFactory(projectileFactory) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    Grid& _grid;
    ProjectileFactory& _projectileFactory;
};

}  // namespace linguine
