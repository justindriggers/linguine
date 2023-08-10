#include "PlayerControllerSystem.h"

#include "components/Ability.h"
#include "components/AbilityButton.h"
#include "components/Cast.h"
#include "components/GlobalCooldown.h"
#include "components/HealthBar.h"
#include "components/Hovered.h"

namespace linguine {

void PlayerControllerSystem::update(float deltaTime) {
  findEntities<GlobalCooldown>()->each([this](const Entity& entity) {
    auto globalCooldown = entity.get<GlobalCooldown>();

    if (globalCooldown->elapsed >= globalCooldown->total) {
      findEntities<HealthBar, Hovered>()->each([this, &globalCooldown](const Entity& healthBarEntity) {
        auto healthBar = healthBarEntity.get<HealthBar>();

        findEntities<Cast>()->each([this, &globalCooldown, &healthBar](const Entity& castEntity) {
          auto cast = castEntity.get<Cast>();

          if (cast->elapsed <= 0.0f) {
            findEntities<AbilityButton>()->each([this, &cast, &globalCooldown, &healthBar](const Entity& abilityButtonEntity) {
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
