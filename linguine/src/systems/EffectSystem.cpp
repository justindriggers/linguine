#include "EffectSystem.h"

#include "components/EffectTracker.h"
#include "components/HealthBar.h"
#include "components/HudDetails.h"
#include "components/Progressable.h"
#include "components/Transform.h"

namespace linguine {

void EffectSystem::update(float deltaTime) {
  auto effectsByHealthBar = std::unordered_map<uint64_t, std::vector<uint64_t>>();

  findEntities<EffectTracker, Progressable>()->each([this, &effectsByHealthBar](const Entity& entity) {
    auto effectTracker = entity.get<EffectTracker>();
    auto progressable = entity.get<Progressable>();
    progressable->feature->progress = 1.0f - effectTracker->timeSinceApplication / effectTracker->effect.getDuration();

    auto target = getEntityById(effectTracker->targetId);
    auto hudDetails = target->get<HudDetails>();

    auto it = effectsByHealthBar.find(hudDetails->healthBarId);

    if (it == effectsByHealthBar.end()) {
      effectsByHealthBar[hudDetails->healthBarId] = { entity.getId() };
    } else {
      it->second.push_back(entity.getId());
    }
  });

  for (const auto& healthBarEffects : effectsByHealthBar) {
    auto healthBarEntity = getEntityById(healthBarEffects.first);
    auto effects = healthBarEffects.second;

    std::sort(effects.begin(), effects.end(), [this](uint64_t a, uint64_t b) {
      auto entityA = getEntityById(a);
      auto entityB = getEntityById(b);

      auto progressableA = entityA->get<Progressable>();
      auto progressableB = entityB->get<Progressable>();

      return progressableA->feature->progress > progressableB->feature->progress;
    });

    auto offenseCount = 0;
    auto defenseCount = 0;

    for (const auto& effectEntityId : effects) {
      auto effectEntity = getEntityById(effectEntityId);
      auto progressable = effectEntity->get<Progressable>();
      progressable->renderable->setEnabled(healthBarEntity->has<HealthBar>());

      auto healthBarTransform = healthBarEntity->get<Transform>();

      auto transform = effectEntity->get<Transform>();
      transform->position.x = healthBarTransform->position.x;

      auto effectTracker = effectEntity->get<EffectTracker>();

      switch (effectTracker->effect.getCategory()) {
      case Offense:
      case Control:
        transform->position.y = healthBarTransform->position.y
                                + healthBarTransform->scale.y / 2.0f
                                - transform->scale.y / 2.0f
                                - 2.0f * static_cast<float>(offenseCount++)
                                - 1.0f;
        break;
      case Defense:
      case Support:
        transform->position.y = healthBarTransform->position.y
                                - healthBarTransform->scale.y / 2.0f
                                + transform->scale.y / 2.0f
                                + 2.0f * static_cast<float>(defenseCount++)
                                + 1.0f;
        break;
      default:
        throw std::runtime_error("Unsupported Effect category");
      }
    }
  }
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
