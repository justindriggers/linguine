#include "CollisionSystem.h"

#include <glm/geometric.hpp>

#include "components/CameraFixture.h"
#include "components/CircleCollider.h"
#include "components/Friendly.h"
#include "components/Hit.h"
#include "components/Hostile.h"
#include "components/PhysicalState.h"
#include "components/Projectile.h"
#include "components/Unit.h"

namespace linguine {

void CollisionSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Hit>()->each([](Entity& entity) {
    entity.remove<Hit>();
  });

  findEntities<Hostile, Projectile, PhysicalState, CircleCollider>()->each([this](Entity& a) {
    findEntities<Friendly, Unit, PhysicalState, CircleCollider>()->each([&a](const Entity& b) {
      detectHit(a, b);
    });
  });

  findEntities<Friendly, Projectile, PhysicalState, CircleCollider>()->each([this](Entity& a) {
    findEntities<Hostile, Unit, PhysicalState, CircleCollider>()->each([&a](const Entity& b) {
      detectHit(a, b);
    });
  });

  findEntities<Hostile, Unit, PhysicalState, CircleCollider>()->each([this](Entity& a) {
    findEntities<Friendly, Unit, PhysicalState, CircleCollider>()->each([&a](const Entity& b) {
      detectHit(a, b);
    });
  });

  findEntities<Friendly, Unit, PhysicalState, CircleCollider>()->each([this](Entity& a) {
    findEntities<Hostile, Unit, PhysicalState, CircleCollider>()->each([&a](const Entity& b) {
      detectHit(a, b);
    });
  });

  findEntities<Projectile, PhysicalState, CircleCollider>()->each([this](Entity& a) {
    findEntities<CameraFixture, PhysicalState, CircleCollider>()->each([&a](const Entity& b) {
      if (!checkCollision(a, b)) {
        a.destroy();
      }
    });
  });
}

bool CollisionSystem::checkCollision(const Entity& a, const Entity& b) {
  if (a.getId() == b.getId()) {
    return false;
  }

  auto positionA = a.get<PhysicalState>()->currentPosition;
  auto positionB = b.get<PhysicalState>()->currentPosition;

  auto colliderA = a.get<CircleCollider>();
  auto colliderB = b.get<CircleCollider>();

  auto distance = glm::distance(positionA, positionB);
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
