#include "PlayerControllerSystem.h"

#include "components/Ability.h"
#include "components/AbilityButton.h"
#include "components/Cast.h"
#include "components/Drawable.h"
#include "components/Friendly.h"
#include "components/GlobalCooldown.h"
#include "components/HealthBar.h"
#include "components/Hovered.h"
#include "components/TargetIndicator.h"
#include "components/Transform.h"

namespace linguine {

void PlayerControllerSystem::update(float deltaTime) {
  findEntities<TargetIndicator, Drawable, Transform>()->each([this](const Entity& entity) {
    auto targetIndicatorDrawable = entity.get<Drawable>();
    targetIndicatorDrawable->renderable->setEnabled(false);

    auto targetIndicatorTransform = entity.get<Transform>();

    findEntities<HealthBar, Hovered, Transform>()->each([&targetIndicatorDrawable, &targetIndicatorTransform](const Entity& healthBarEntity) {
      auto healthBarTransform = healthBarEntity.get<Transform>();
      targetIndicatorTransform->position = healthBarTransform->position;

      targetIndicatorDrawable->renderable->setEnabled(true);
    });
  });

  findEntities<GlobalCooldown>()->each([this](const Entity& entity) {
    auto globalCooldown = entity.get<GlobalCooldown>();

    if (globalCooldown->elapsed >= globalCooldown->total) {
      findEntities<HealthBar, Hovered>()->each([this, &globalCooldown](const Entity& healthBarEntity) {
        auto healthBar = healthBarEntity.get<HealthBar>();

        findEntities<Cast>()->each([this, &globalCooldown, &healthBar](const Entity& castEntity) {
          auto cast = castEntity.get<Cast>();

          if (cast->elapsed <= 0.0f) {
            findEntities<Friendly, AbilityButton>()->each([this, &cast, &globalCooldown, &healthBar](const Entity& abilityButtonEntity) {
              auto abilityButton = abilityButtonEntity.get<AbilityButton>();
              auto abilityEntity = getEntityById(abilityButton->abilityEntityId);
              auto ability = abilityEntity->get<Ability>();

              if (_inputManager.isKeyPressed(abilityButton->key) && ability->remainingCooldown <= 0.0f) {
                cast->abilityEntityId = abilityButton->abilityEntityId;
                cast->targetEntityId = healthBar->entityId;
                globalCooldown->elapsed = 0.0f;
              }
            });
          }
        });
      });
    }
  });
}

}  // namespace linguine
