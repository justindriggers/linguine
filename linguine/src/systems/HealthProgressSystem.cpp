#include "HealthProgressSystem.h"

#include "components/Health.h"
#include "components/HealthBar.h"
#include "components/Progressable.h"

namespace linguine {

void HealthProgressSystem::update(float deltaTime) {
  findEntities<Health, Progressable>()->each([](const Entity& entity) {
    auto health = entity.get<Health>();
    auto feature = entity.get<Progressable>()->feature;

    feature->progress = glm::clamp(
        static_cast<float>(health->current) / static_cast<float>(health->max),
        0.0f,
        1.0f
    );

    if (feature->progress > 0.0f) {
      feature->backgroundColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    } else {
      feature->backgroundColor = { 0.0f, 0.0f, 0.0f, 0.5f };
    }
  });

  findEntities<HealthBar, Progressable>()->each([this](Entity& healthBarEntity) {
    auto healthBar = healthBarEntity.get<HealthBar>();
    auto feature = healthBarEntity.get<Progressable>()->feature;
    auto entity = getEntityById(healthBar->entityId);

    if (entity->has<Health>()) {
      auto health = entity->get<Health>();

      feature->progress = glm::clamp(
          static_cast<float>(health->current) / static_cast<float>(health->max),
          0.0f,
          1.0f
      );

      if (feature->progress > 0.0f) {
        feature->backgroundColor = { 0.0f, 0.0f, 0.0f, 1.0f };
      } else {
        feature->backgroundColor = { 0.0f, 0.0f, 0.0f, 0.5f };
      }
    } else {
      feature->progress = 0.0f;
    }
  });
}

}  // namespace linguine
