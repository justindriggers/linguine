#include "ProjectileSystem.h"

#include <glm/common.hpp>
#include <glm/geometric.hpp>

#include "components/Health.h"
#include "components/Hit.h"
#include "components/PhysicalState.h"
#include "components/Projectile.h"

namespace linguine {

void ProjectileSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Projectile, Hit>()->each([this](Entity& entity) {
    auto projectile = entity.get<Projectile>();

    auto hit = entity.get<Hit>();

    for (const auto entityId : hit->entityIds) {
      if (entityId == projectile->target) {
        auto target = getEntityById(entityId);
        entity.destroy();
      }
    }
  });

  findEntities<Projectile, PhysicalState>()->each([this, fixedDeltaTime](Entity& entity) {
    auto projectile = entity.get<Projectile>();
    auto physicalState = entity.get<PhysicalState>();

    auto target = getEntityById(projectile->target);

    if (target->has<PhysicalState>()) {
      auto targetPosition = target->get<PhysicalState>()->currentPosition;
      auto targetDirection = glm::normalize(targetPosition - physicalState->currentPosition);
      physicalState->currentPosition += targetDirection * projectile->speed * fixedDeltaTime;
    } else {
      entity.destroy();
    }
  });
}

}  // namespace linguine
