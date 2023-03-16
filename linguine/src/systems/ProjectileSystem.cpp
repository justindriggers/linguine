#include "ProjectileSystem.h"

#include <glm/common.hpp>

#include "components/Health.h"
#include "components/Hit.h"
#include "components/PhysicalState.h"
#include "components/Projectile.h"

namespace linguine {

void ProjectileSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Projectile, Hit>()->each([this](Entity& entity) {
    auto projectile = entity.get<Projectile>();
    auto power = projectile->power;

    auto hit = entity.get<Hit>();

    for (const auto entityId : hit->entityIds) {
      auto target = getEntityById(entityId);
      auto health = target->get<Health>();
      health->current = glm::clamp<int32_t>(health->current - power, 0, health->max);
    }

    entity.destroy();
  });

  findEntities<Projectile, PhysicalState>()->each([fixedDeltaTime](const Entity& entity) {
    auto projectile = entity.get<Projectile>();
    auto physicalState = entity.get<PhysicalState>();

    physicalState->currentPosition += projectile->velocity * fixedDeltaTime;
  });
}

}  // namespace linguine
