#pragma once

#include "System.h"

#include "data/RaycastHit.h"
#include "physics/World.h"

namespace linguine {

class CollisionSystem : public System {
  public:
    explicit CollisionSystem(EntityManager& entityManager,
                             const std::shared_ptr<physics::World>& world)
        : System(entityManager), _world(world) {}

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

  private:
    std::shared_ptr<physics::World> _world;
};

}  // namespace linguine
