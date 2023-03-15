#include "CollisionSystem.h"

#include "components/CircleCollider.h"
#include "components/Friendly.h"
#include "components/Hit.h"
#include "components/Hostile.h"
#include "components/Projectile.h"
#include "components/Transform.h"
#include "components/Unit.h"

namespace linguine {

void CollisionSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Hit>()->each([](Entity& entity) {
    entity.remove<Hit>();
  });

  findEntities<Hostile, Projectile, Transform, CircleCollider>()->each([this](Entity& a) {
    findEntities<Friendly, Unit, Transform, CircleCollider>()->each([&a](const Entity& b) {
      detectHit(a, b);
    });
  });

  findEntities<Friendly, Projectile, Transform, CircleCollider>()->each([this](Entity& a) {
    findEntities<Hostile, Unit, Transform, CircleCollider>()->each([&a](const Entity& b) {
      detectHit(a, b);
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

void CollisionSystem::detectHit(Entity& a, const Entity& b) {
  if (checkCollision(a, b)) {
    if (a.has<Hit>()) {
      a.get<Hit>()->entityIds.push_back(b.getId());
    } else {
      a.add<Hit>()->entityIds = { b.getId() };
    }
  }
}

}  // namespace linguine
