#include "EffectSystem.h"

#include "components/EffectTracker.h"
#include "components/HealthBar.h"
#include "components/Progressable.h"
#include "components/Transform.h"

namespace linguine {

void EffectSystem::update(float deltaTime) {
  findEntities<EffectTracker, Progressable>()->each([this](const Entity& entity) {
    auto effectTracker = entity.get<EffectTracker>();
    auto progressable = entity.get<Progressable>();
    progressable->feature->progress = 1.0f - effectTracker->timeSinceApplication / effectTracker->effect.getDuration();

    findEntities<HealthBar>()->each([&entity, &effectTracker](const Entity& healthBarEntity) {
      auto healthBar = healthBarEntity.get<HealthBar>();

      if (healthBar->entityId == effectTracker->targetId) {
        auto healthBarTransform = healthBarEntity.get<Transform>();

        auto transform = entity.get<Transform>();
        transform->position.x = healthBarTransform->position.x;
        transform->position.y = healthBarTransform->position.y - healthBarTransform->scale.y / 2.0f - transform->scale.y / 2.0f;
      }
    });
  });
}

void EffectSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<EffectTracker>()->each([fixedDeltaTime](Entity& entity) {
    auto effectTracker = entity.get<EffectTracker>();
    effectTracker->timeSinceLastTick += fixedDeltaTime;
    effectTracker->timeSinceApplication += fixedDeltaTime;
    effectTracker->effect.update(effectTracker);

    if (effectTracker->ticks >= effectTracker->effect.getTicks()) {
      effectTracker->effect.onRemove(effectTracker);
      entity.destroy();
    }
  });
}

}  // namespace linguine
