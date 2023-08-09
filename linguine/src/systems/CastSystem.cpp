#include "CastSystem.h"

#include <glm/gtx/norm.hpp>

#include "components/Ability.h"
#include "components/Cast.h"
#include "components/GlobalCooldown.h"
#include "components/Player.h"
#include "components/Progressable.h"
#include "components/Velocity.h"

namespace linguine {

void CastSystem::update(float deltaTime) {
  findEntities<Cast, Progressable>()->each([this, deltaTime](Entity& entity) {
    auto cast = entity.get<Cast>();

    if (cast->abilityEntityId) {
      auto progressable = entity.get<Progressable>();

      auto abilityEntity = getEntityById(*cast->abilityEntityId);
      auto ability = abilityEntity->get<Ability>();

      if (ability->spell.castTime > 0.0f) {
        auto isMoving = false;

        findEntities<Player, Velocity>()->each([&isMoving](const Entity& entity) {
          auto velocity = entity.get<Velocity>();
          isMoving = glm::length2(velocity->velocity) > 0.0f;
        });

        if (isMoving) {
          cast->abilityEntityId = {};
          cast->targetEntityId = {};
          cast->elapsed = 0.0f;

          findEntities<GlobalCooldown>()->each([](const Entity& gcdEntity) {
            auto globalCooldown = gcdEntity.get<GlobalCooldown>();
            globalCooldown->elapsed = globalCooldown->total;
          });

          progressable->renderable->setEnabled(false);

          return;
        }
      }

      cast->elapsed += deltaTime;

      if (cast->elapsed >= ability->spell.castTime) {
        auto target = getEntityById(*cast->targetEntityId);
        ability->spell.action->execute(*target);
        ability->remainingCooldown = ability->spell.cooldown;
        cast->abilityEntityId = {};
        cast->targetEntityId = {};
        cast->elapsed = 0.0f;
        progressable->renderable->setEnabled(false);
      } else {
        progressable->feature->progress = cast->elapsed / ability->spell.castTime;
        progressable->renderable->setEnabled(true);
      }
    }
  });
}

}  // namespace linguine
