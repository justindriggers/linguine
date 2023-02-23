#pragma once

#include "System.h"

#include "Logger.h"

namespace linguine {

class FpsSystem : public System {
  public:
    FpsSystem(EntityManager& entityManager, Logger& logger)
        : System(entityManager), _logger(logger) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    Logger& _logger;

    float _dtAccumulator = 0.0f;
    int _updateCounter = 0;

    float _fdtAccumulator = 0.0f;
    int _fixedUpdateCounter = 0;
};

}  // namespace linguine
