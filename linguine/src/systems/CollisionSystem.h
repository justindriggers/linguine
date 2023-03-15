#pragma once

#include "System.h"

namespace linguine {

class CollisionSystem : public System {
  public:
    explicit CollisionSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    static bool checkCollision(const Entity& a, const Entity& b);

    static void detectHit(Entity& a, const Entity& b);
};

}  // namespace linguine
