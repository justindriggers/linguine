#pragma once

#include "System.h"

#include "TimeManager.h"

namespace linguine {

class OrbitSystem : public System {
  public:
    OrbitSystem(EntityManager& entityManager, TimeManager& timeManager)
        : System(entityManager), _timeManager(timeManager) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    TimeManager& _timeManager;
};

}  // namespace linguine
