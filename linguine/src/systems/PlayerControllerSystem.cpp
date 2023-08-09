#include "PlayerControllerSystem.h"

#include "components/Ability.h"
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
            findEntities<Ability>()->each([this, &cast, &globalCooldown, &healthBar](const Entity& abilityEntity) {
              auto ability = abilityEntity.get<Ability>();

              if (_inputManager.isKeyPressed(ability->key) && ability->remainingCooldown <= 0.0f) {
                cast->abilityEntityId = abilityEntity.getId();
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
