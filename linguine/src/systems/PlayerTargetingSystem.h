#pragma once

#include "System.h"

#include <random>

#include "components/GridPosition.h"
#include "components/Targeting.h"
#include "data/Grid.h"

namespace linguine {

class PlayerTargetingSystem : public System {
  public:
    PlayerTargetingSystem(EntityManager& entityManager, Grid& grid)
        : System(entityManager), _grid(grid) {}

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    Grid& _grid;
    std::random_device _random;

    void clearTargetIfDead(Component<Targeting>& targeting);

    void selectTarget(Component<Targeting>& targeting,
                      Component<GridPosition>& gridPosition,
                      const std::vector<std::shared_ptr<Entity>>& availableTargets);
};

}  // namespace linguine
