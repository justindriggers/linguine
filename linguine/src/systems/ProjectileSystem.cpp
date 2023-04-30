#include "ProjectileSystem.h"

#include <glm/common.hpp>
#include <glm/geometric.hpp>

#include "components/Friendly.h"
#include "components/Health.h"
#include "components/Hit.h"
#include "components/PhysicalState.h"
#include "components/PlayerTarget.h"
#include "components/Projectile.h"

namespace linguine {

void ProjectileSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Projectile, Hit>()->each([this](Entity& entity) {
    auto projectile = entity.get<Projectile>();

    auto hit = entity.get<Hit>();

    for (const auto entityId : hit->entityIds) {
      if (entityId == projectile->target) {
        auto target = getEntityById(entityId);

        if (target->has<Friendly>()) {
          auto friendlies = findEntities<Friendly, Health>()->get();

          if (!friendlies.empty()) {
            auto randomEntity = std::uniform_int_distribution<>(0, static_cast<int>(friendlies.size() - 1));
            const auto targetIndex = randomEntity(_random);
            auto health = friendlies[targetIndex]->get<Health>();
            health->current = glm::clamp<int32_t>(health->current - projectile->power, 0, health->max);
          }

          findEntities<PlayerTarget>()->each([&projectile](const Entity& playerTargetEntity) {
            auto playerTarget = playerTargetEntity.get<PlayerTarget>();

            if (!playerTarget->entityId) {
              playerTarget->entityId = projectile->actor;
            }
          });
        } else {
          auto health = target->get<Health>();
          health->current = glm::clamp<int32_t>(health->current - projectile->power, 0, health->max);
        }

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
