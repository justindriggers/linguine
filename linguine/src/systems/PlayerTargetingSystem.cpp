#include "PlayerTargetingSystem.h"

#include "components/Alive.h"
#include "components/Drawable.h"
#include "components/Hostile.h"
#include "components/PlayerTarget.h"
#include "components/Tapped.h"
#include "components/Transform.h"

namespace linguine {

void PlayerTargetingSystem::update(float deltaTime) {
  findEntities<Hostile, Tapped>()->each([this](const Entity& hostileEntity) {
    findEntities<PlayerTarget>()->each([&hostileEntity](const Entity& playerTargetEntity) {
      auto playerTarget = playerTargetEntity.get<PlayerTarget>();
      playerTarget->entityId = hostileEntity.getId();
    });
  });

  findEntities<PlayerTarget, Drawable, Transform>()->each([this](const Entity& entity) {
    auto playerTarget = entity.get<PlayerTarget>();

    if (playerTarget->entityId) {
      auto target = getEntityById(*playerTarget->entityId);

      if (!target->has<Alive>()) {
        playerTarget->entityId = {};
      }

      auto hasTarget = playerTarget->entityId.has_value();

      auto drawable = entity.get<Drawable>();
      drawable->renderable->setEnabled(hasTarget);

      if (hasTarget) {
        auto transform = entity.get<Transform>();
        auto targetTransform = target->get<Transform>();

        transform->position.x = targetTransform->position.x;
        transform->position.y = targetTransform->position.y;
      }
    }
  });
}

}  // namespace linguine
