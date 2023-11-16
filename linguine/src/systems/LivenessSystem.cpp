#include "LivenessSystem.h"

#include "components/Alive.h"
#include "components/EffectTracker.h"
#include "components/Health.h"
#include "components/Score.h"
#include "scenes/ShopScene.h"

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

  auto living = findEntities<Health, Alive>()->get();

  if (living.empty()) {
    findEntities<Score>()->each([this](const Entity& entity) {
      auto score = entity.get<Score>();
      _saveManager.addPoints(score->points);
      _serviceLocator.get<SceneManager>().load(std::make_unique<ShopScene>(_serviceLocator));
    });
  }
}

}  // namespace linguine
