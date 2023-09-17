#include "PlayerControllerSystem.h"

#include "components/Ability.h"
#include "components/AbilityButton.h"
#include "components/Cast.h"
#include "components/Drawable.h"
#include "components/Friendly.h"
#include "components/GlobalCooldown.h"
#include "components/HealthBar.h"
#include "components/Pressed.h"
#include "components/Selected.h"
#include "components/Tapped.h"
#include "components/TargetIndicator.h"
#include "components/Transform.h"

namespace linguine {

void PlayerControllerSystem::update(float deltaTime) {
  findEntities<TargetIndicator, Drawable, Transform>()->each([this](const Entity& entity) {
    auto targetIndicatorDrawable = entity.get<Drawable>();
    auto targetIndicatorTransform = entity.get<Transform>();

    findEntities<HealthBar, Tapped, Transform>()->each([&targetIndicatorDrawable, &targetIndicatorTransform, this](Entity& healthBarEntity) {
      findEntities<HealthBar, Selected>()->each([&healthBarEntity](Entity& selectedHealthBarEntity) {
        if (healthBarEntity.getId() != selectedHealthBarEntity.getId()) {
          selectedHealthBarEntity.remove<Selected>();
        }
      });

      if (!healthBarEntity.has<Selected>()) {
        healthBarEntity.add<Selected>();
      }

      auto healthBarTransform = healthBarEntity.get<Transform>();
      targetIndicatorTransform->position = healthBarTransform->position;

      targetIndicatorDrawable->renderable->setEnabled(true);
    });
  });

  findEntities<HealthBar, Selected>()->each([this](const Entity& healthBarEntity) {
    auto healthBar = healthBarEntity.get<HealthBar>();

    findEntities<Cast>()->each([this, &healthBar](const Entity& castEntity) {
      auto cast = castEntity.get<Cast>();
      auto isCanceled = true;

      findEntities<Friendly, AbilityButton, Pressed>()->each([this, &cast, &healthBar, &isCanceled](const Entity& abilityButtonEntity) {
        auto abilityButton = abilityButtonEntity.get<AbilityButton>();
        auto abilityEntity = getEntityById(abilityButton->abilityEntityId);
        auto ability = abilityEntity->get<Ability>();
        auto pressed = abilityButtonEntity.get<Pressed>();

        if (!cast->abilityEntityId && ability->remainingCooldown <= 0.0f && pressed->isFirstFrame) {
          findEntities<GlobalCooldown>()->each([&cast, &abilityButton, &healthBar, &isCanceled](const Entity& entity) {
            auto globalCooldown = entity.get<GlobalCooldown>();

            if (globalCooldown->remaining <= 0.0f) {
              cast->abilityEntityId = abilityButton->abilityEntityId;
              cast->targetEntityId = healthBar->entityId;
              globalCooldown->remaining = globalCooldown->total;
              isCanceled = false;
            }
          });
        } else if (cast->abilityEntityId == abilityEntity->getId()) {
          isCanceled = false;
        }
      });

      if (cast->abilityEntityId && isCanceled) {
        cast->abilityEntityId = {};
        cast->targetEntityId = {};
        cast->elapsed = 0.0f;

        findEntities<GlobalCooldown>()->each([](const Entity& entity) {
          auto globalCooldown = entity.get<GlobalCooldown>();
          globalCooldown->remaining = 0.0f;
        });
      }
    });
  });
}

}  // namespace linguine
