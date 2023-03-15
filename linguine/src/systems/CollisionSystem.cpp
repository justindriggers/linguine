#include "CollisionSystem.h"

#include "components/CircleCollider.h"
#include "components/Friendly.h"
#include "components/Hostile.h"
#include "components/Projectile.h"
#include "components/Transform.h"
#include "components/Unit.h"

namespace linguine {

void CollisionSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Hostile, Projectile, Transform, CircleCollider>()->each([this](const Entity& a) {
    findEntities<Friendly, Unit, Transform, CircleCollider>()->each([this, &a](const Entity& b) {
      if (checkCollision(a, b)) {
        _logger.log("Hostile Projectile " + std::to_string(a.getId()) + " is colliding with Friendly Unit " + std::to_string(b.getId()));
      }
    });
  });

  findEntities<Friendly, Projectile, Transform, CircleCollider>()->each([this](const Entity& a) {
    findEntities<Hostile, Unit, Transform, CircleCollider>()->each([this, &a](const Entity& b) {
      if (checkCollision(a, b)) {
        _logger.log("Friendly Projectile " + std::to_string(a.getId()) + " is colliding with Hostile Unit " + std::to_string(b.getId()));
      }
    });
  });
}

bool CollisionSystem::checkCollision(const Entity& a, const Entity& b) {
  if (a.getId() == b.getId()) {
    return false;
  }

  auto transformA = a.get<Transform>();
  auto transformB = b.get<Transform>();

  auto colliderA = a.get<CircleCollider>();
  auto colliderB = b.get<CircleCollider>();

  auto distance = glm::distance(transformA->position, transformB->position);
  return distance <= colliderA->radius + colliderB->radius;
}

}  // namespace linguine
