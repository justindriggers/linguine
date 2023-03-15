#include "ProjectileSystem.h"

#include "components/Hit.h"
#include "components/PhysicalState.h"
#include "components/Projectile.h"

namespace linguine {

void ProjectileSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Projectile, Hit>()->each([this](Entity& entity) {
    auto hit = entity.get<Hit>();

    for (const auto entityId : hit->entityIds) {
      _logger.log("Projectile " + std::to_string(entity.getId()) + " hit Unit " + std::to_string(entityId));
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
