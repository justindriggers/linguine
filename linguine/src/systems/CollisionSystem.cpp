#include "CollisionSystem.h"

#include <glm/geometric.hpp>

#include "components/BoxCollider.h"
#include "components/CameraFixture.h"
#include "components/CircleCollider.h"
#include "components/Hit.h"
#include "components/PhysicalState.h"
#include "components/Projectile.h"
#include "components/Static.h"
#include "components/Trigger.h"

namespace linguine {

void CollisionSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Hit>()->each([](Entity& entity) {
    entity.remove<Hit>();
  });

  findEntities<PhysicalState>()->each([this](Entity& a) {
    findEntities<PhysicalState>()->each([&a](Entity& b) {
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

  if (a.has<Static>() && b.has<Static>()) {
    return false;
  }

  if (a.has<BoxCollider>() && b.has<BoxCollider>()
      && checkBoxBoxCollision(a, b)) {
    return true;
  }

  if (a.has<BoxCollider>() && b.has<CircleCollider>()
      && checkBoxCircleCollision(a, b)) {
    return true;
  }

  if (a.has<CircleCollider>() && b.has<BoxCollider>()
      && checkBoxCircleCollision(b, a)) {
    return true;
  }

  if (a.has<CircleCollider>() && b.has<CircleCollider>()
      && checkCircleCircleCollision(a, b)) {
    return true;
  }

  return false;
}

bool CollisionSystem::checkBoxBoxCollision(const Entity& a, const Entity& b) {
  auto positionA = a.get<PhysicalState>()->currentPosition;
  auto positionB = b.get<PhysicalState>()->currentPosition;

  auto colliderA = a.get<BoxCollider>();
  auto colliderB = b.get<BoxCollider>();

  return positionA.x + colliderA->size.x / 2.0f >= positionB.x - colliderB->size.x / 2.0f
         && positionA.x - colliderA->size.x / 2.0f <= positionB.x + colliderB->size.x / 2.0f
         && positionA.y + colliderA->size.y / 2.0f >= positionB.y - colliderB->size.y / 2.0f
         && positionA.y - colliderA->size.y / 2.0f <= positionB.y + colliderB->size.y / 2.0f;
}

bool CollisionSystem::checkBoxCircleCollision(const Entity& a, const Entity& b) {
  auto positionA = a.get<PhysicalState>()->currentPosition;
  auto positionB = b.get<PhysicalState>()->currentPosition;

  auto colliderA = a.get<BoxCollider>();
  auto colliderB = b.get<CircleCollider>();

  return positionA.x + colliderA->size.x / 2.0f >= positionB.x - colliderB->radius
         && positionA.x - colliderA->size.x / 2.0f <= positionB.x + colliderB->radius
         && positionA.y + colliderA->size.y / 2.0f >= positionB.y - colliderB->radius
         && positionA.y - colliderA->size.y / 2.0f <= positionB.y + colliderB->radius;
}

bool CollisionSystem::checkCircleCircleCollision(const Entity& a, const Entity& b) {
  auto positionA = a.get<PhysicalState>()->currentPosition;
  auto positionB = b.get<PhysicalState>()->currentPosition;

  auto colliderA = a.get<CircleCollider>();
  auto colliderB = b.get<CircleCollider>();

  auto distance = glm::distance(positionA, positionB);
  return distance <= colliderA->radius + colliderB->radius;
}

void CollisionSystem::resolveCollision(const Entity& a, const Entity& b) {
  if (a.has<BoxCollider>() && b.has<BoxCollider>()) {
    resolveBoxBoxCollision(a, b);
  } else if (a.has<BoxCollider>() && b.has<CircleCollider>()) {
    resolveBoxCircleCollision(a, b);
  } else if (a.has<CircleCollider>() && b.has<BoxCollider>()) {
    resolveBoxCircleCollision(b, a);
  } else if (a.has<CircleCollider>() && b.has<CircleCollider>()) {
    resolveCircleCircleCollision(a, b);
  }
}

void CollisionSystem::resolveBoxBoxCollision(const Entity& a, const Entity& b) {
  auto stateA = a.get<PhysicalState>();
  auto stateB = b.get<PhysicalState>();

  auto colliderA = a.get<BoxCollider>();
  auto colliderB = b.get<BoxCollider>();

  auto offset = stateA->currentPosition - stateB->currentPosition;
  auto penetration = colliderA->size / 2.0f + colliderB->size / 2.0f - glm::abs(offset);

  if (glm::abs(penetration.x) < glm::abs(penetration.y)) {
    auto resolutionX = glm::sign(offset.x) * penetration.x;

    if (!a.has<Static>() && !b.has<Static>()) {
      stateA->currentPosition.x += resolutionX / 2.0f;
      stateB->currentPosition.x -= resolutionX / 2.0f;
    } else if (a.has<Static>() && !b.has<Static>()) {
      stateB->currentPosition.x -= resolutionX;
    } else {
      stateA->currentPosition.x += resolutionX;
    }
  } else {
    auto resolutionY = glm::sign(offset.y) * penetration.y;

    if (!a.has<Static>() && !b.has<Static>()) {
      stateA->currentPosition.y += resolutionY / 2.0f;
      stateB->currentPosition.y -= resolutionY / 2.0f;
    } else if (a.has<Static>() && !b.has<Static>()) {
      stateB->currentPosition.y -= resolutionY;
    } else {
      stateA->currentPosition.y += resolutionY;
    }
  }
}

void CollisionSystem::resolveBoxCircleCollision(const Entity& a, const Entity& b) {
  auto stateA = a.get<PhysicalState>();
  auto stateB = b.get<PhysicalState>();

  auto colliderA = a.get<BoxCollider>();
  auto colliderB = b.get<CircleCollider>();

  auto offset = stateA->currentPosition - stateB->currentPosition;
  auto penetration = colliderA->size / 2.0f + colliderB->radius - glm::abs(offset);

  if (glm::abs(penetration.x) < glm::abs(penetration.y)) {
    auto resolutionX = glm::sign(offset.x) * penetration.x;

    if (!a.has<Static>() && !b.has<Static>()) {
      stateA->currentPosition.x += resolutionX / 2.0f;
      stateB->currentPosition.x -= resolutionX / 2.0f;
    } else if (a.has<Static>() && !b.has<Static>()) {
      stateB->currentPosition.x -= resolutionX;
    } else {
      stateA->currentPosition.x += resolutionX;
    }
  } else {
    auto resolutionY = glm::sign(offset.y) * penetration.y;

    if (!a.has<Static>() && !b.has<Static>()) {
      stateA->currentPosition.y += resolutionY / 2.0f;
      stateB->currentPosition.y -= resolutionY / 2.0f;
    } else if (a.has<Static>() && !b.has<Static>()) {
      stateB->currentPosition.y -= resolutionY;
    } else {
      stateA->currentPosition.y += resolutionY;
    }
  }
}

void CollisionSystem::resolveCircleCircleCollision(const Entity& a, const Entity& b) {
  auto stateA = a.get<PhysicalState>();
  auto stateB = b.get<PhysicalState>();

  auto colliderA = a.get<CircleCollider>();
  auto colliderB = b.get<CircleCollider>();

  auto offset = stateA->currentPosition - stateB->currentPosition;
  auto direction = glm::normalize(offset);
  auto penetration = colliderA->radius + colliderB->radius - glm::abs(offset);

  auto resolution = direction * penetration;

  if (!a.has<Static>() && !b.has<Static>()) {
    stateA->currentPosition += resolution / 2.0f;
    stateB->currentPosition -= resolution / 2.0f;
  } else if (a.has<Static>() && !b.has<Static>()) {
    stateB->currentPosition -= resolution;
  } else {
    stateA->currentPosition += resolution;
  }
}

void CollisionSystem::detectHit(Entity& a, Entity& b) {
  if (checkCollision(a, b)) {
    if (a.has<Trigger>()) {
      if (a.has<Hit>()) {
        a.get<Hit>()->entityIds.push_back(b.getId());
      } else {
        a.add<Hit>()->entityIds = { b.getId() };
      }
    }

    if (b.has<Trigger>()) {
      if (b.has<Hit>()) {
        b.get<Hit>()->entityIds.push_back(a.getId());
      } else {
        b.add<Hit>()->entityIds = { a.getId() };
      }
    }

    if (!a.has<Trigger>() && !b.has<Trigger>()) {
      resolveCollision(a, b);
    }
  }
}

}  // namespace linguine
