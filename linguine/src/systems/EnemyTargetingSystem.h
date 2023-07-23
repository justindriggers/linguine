#pragma once

#include "System.h"

#include <random>

#include "components/PhysicalState.h"
#include "components/Targeting.h"

namespace linguine {

class EnemyTargetingSystem : public System {
  public:
    explicit EnemyTargetingSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    std::random_device _random;

    void clearTargetIfDead(Component<Targeting>& targeting);

    void selectTarget(Component<Targeting>& targeting,
                      Component<PhysicalState>& physicalState,
                      const std::vector<std::shared_ptr<Entity>>& availableTargets);

    void moveTowardTarget(Component<Targeting>& targeting,
                          Component<PhysicalState>& physicalState,
                          float fixedDeltaTime);
};

}  // namespace linguine
