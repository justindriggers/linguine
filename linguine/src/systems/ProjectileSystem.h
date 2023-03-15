#pragma once

#include "System.h"

#include "Logger.h"

namespace linguine {

class ProjectileSystem : public System {
  public:
    ProjectileSystem(EntityManager& entityManager,
                     Logger& logger)
        : System(entityManager), _logger(logger) {}

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    Logger& _logger;
};

}  // namespace linguine
