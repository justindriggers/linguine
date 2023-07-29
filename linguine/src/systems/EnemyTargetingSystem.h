#pragma once

#include "System.h"

#include <random>

#include "components/PhysicalState.h"
#include "components/Raycaster.h"
#include "components/Targeting.h"
#include "data/Grid.h"

namespace linguine {

class EnemyTargetingSystem : public System {
  public:
    EnemyTargetingSystem(EntityManager& entityManager, Grid& grid)
        : System(entityManager), _grid(grid) {}

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    Grid& _grid;
    std::random_device _random;

    void clearTargetIfDead(Component<Targeting>& targeting);

    void selectTarget(Component<Targeting>& targeting,
                      Component<PhysicalState>& physicalState,
                      const std::vector<std::shared_ptr<Entity>>& availableTargets);

    void moveTowardTarget(Entity& entity,
                          Component<Targeting>& targeting,
                          Component<PhysicalState>& physicalState,
                          float fixedDeltaTime);
};

}  // namespace linguine
