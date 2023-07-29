#pragma once

#include "System.h"

#include "data/RaycastHit.h"

namespace linguine {

class CollisionSystem : public System {
  public:
    explicit CollisionSystem(EntityManager& entityManager)
        : System(entityManager) {}

    void update(float deltaTime) override {}

    void fixedUpdate(float fixedDeltaTime) override;

  private:
    static bool checkCollision(const Entity& a, const Entity& b);

    static bool checkBoxBoxCollision(const Entity& a, const Entity& b);

    static bool checkBoxCircleCollision(const Entity& a, const Entity& b);

    static bool checkCircleCircleCollision(const Entity& a, const Entity& b);

    static void resolveCollision(const Entity& a, const Entity& b);

    static void resolveBoxBoxCollision(const Entity& a, const Entity& b);

    static void resolveBoxCircleCollision(const Entity& a, const Entity& b);

    static void resolveCircleCircleCollision(const Entity& a, const Entity& b);

    static std::optional<RaycastHit> checkRayIntersection(const Entity& a, const Entity& b);

    static std::optional<RaycastHit> checkRayBoxIntersection(const Entity& a, const Entity& b);

    static std::optional<RaycastHit> checkRayCircleIntersection(const Entity& a, const Entity& b);

    static void detectHit(Entity& a, Entity& b);
};

}  // namespace linguine
