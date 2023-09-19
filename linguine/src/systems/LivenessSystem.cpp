#include "LivenessSystem.h"

#include "components/Alive.h"
#include "components/EffectTracker.h"
#include "components/Health.h"

namespace linguine {

void LivenessSystem::update(float deltaTime) {
  findEntities<Health, Alive>()->each([this](Entity& entity) {
    auto health = entity.get<Health>();

    if (health->current <= 0) {
      findEntities<EffectTracker>()->each([entity](Entity& effectTrackerEntity) {
        auto effectTracker = effectTrackerEntity.get<EffectTracker>();

        if (effectTracker->targetId == entity.getId()) {
          effectTrackerEntity.destroy();
        }
      });

      entity.remove<Alive>();
    }
  });
}

}  // namespace linguine
