#include "CastSystem.h"

#include <glm/gtx/norm.hpp>

#include "components/Ability.h"
#include "components/Alive.h"
#include "components/Cast.h"
#include "components/Circle.h"
#include "components/Transform.h"

namespace linguine {

void CastSystem::update(float deltaTime) {
  findEntities<Cast>()->each([this, deltaTime](Entity& entity) {
    auto cast = entity.get<Cast>();

    if (cast->abilityEntityId) {
      auto abilityEntity = getEntityById(*cast->abilityEntityId);
      auto ability = abilityEntity->get<Ability>();

      cast->elapsed += deltaTime;

      if (cast->elapsed >= ability->spell.castTime) {
        auto target = getEntityById(*cast->targetEntityId);

        if (target->has<Alive>()) {
          ability->spell.action->execute(*target);
          ability->remainingCooldown = ability->spell.cooldown;
        }

        cast->abilityEntityId = {};
        cast->targetEntityId = {};
        cast->elapsed = 0.0f;
      }
    }
  });

  findEntities<Cast, Transform, Circle>()->each([this](Entity& entity) {
    auto cast = entity.get<Cast>();
    auto circle = entity.get<Circle>();

    if (cast->abilityEntityId) {
      auto transform = entity.get<Transform>();

      auto abilityEntity = getEntityById(*cast->abilityEntityId);
      auto ability = abilityEntity->get<Ability>();

      auto progress = cast->elapsed / ability->spell.castTime;
      transform->scale = { 40.0f * progress, 40.0f * progress, 0.0f };
    }

    circle->renderable->setEnabled(cast->abilityEntityId.has_value());
  });
}

}  // namespace linguine
