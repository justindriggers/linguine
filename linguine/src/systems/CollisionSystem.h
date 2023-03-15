#pragma once

#include "System.h"

#include "Logger.h"

namespace linguine {

class CollisionSystem : public System {
  public:
    CollisionSystem(EntityManager& entityManager,
                    Logger& logger)
        : System(entityManager), _logger(logger) {}

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    Logger& _logger;

    static bool checkCollision(const Entity& a, const Entity& b);
};

}  // namespace linguine
