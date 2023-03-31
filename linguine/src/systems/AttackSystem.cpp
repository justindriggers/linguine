#include "AttackSystem.h"

#include <glm/gtx/norm.hpp>

#include "components/ExplosionAttack.h"
#include "components/GridPosition.h"
#include "components/Health.h"
#include "components/MeleeAttack.h"
#include "components/PhysicalState.h"
#include "components/Targeting.h"

namespace linguine {

void AttackSystem::update(float deltaTime) {
  findEntities<MeleeAttack, Targeting, GridPosition>()->each([this, deltaTime](const Entity& entity) {
    auto targeting = entity.get<Targeting>();
    auto meleeAttack = entity.get<MeleeAttack>();
    meleeAttack->elapsed += deltaTime;

    if (targeting->current) {
      auto target = getEntityById(*targeting->current);

      if (target->has<GridPosition>()) {
        auto targetPosition = target->get<GridPosition>()->position;

        auto position = entity.get<GridPosition>()->position;

        if (glm::length2(targetPosition - position) <= 1.0f
            && meleeAttack->elapsed >= meleeAttack->speed) {
          meleeAttack->elapsed = 0.0f;

          auto health = target->get<Health>();
          health->current = glm::clamp<int32_t>(health->current - meleeAttack->power, 0, health->max);
        }
      }
    }
  });
}

void AttackSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<ExplosionAttack, Targeting, PhysicalState>()->each([this, fixedDeltaTime](const Entity& entity) {
    auto targeting = entity.get<Targeting>();
    auto explosionAttack = entity.get<ExplosionAttack>();
    explosionAttack->elapsed += fixedDeltaTime;

    while (targeting->current && explosionAttack->elapsed >= explosionAttack->frequency) {
      explosionAttack->elapsed = 0.0f;

      auto physicalState = entity.get<PhysicalState>();
      auto direction = glm::angleAxis(physicalState->currentRotation, glm::vec3(0.0f, 0.0f, 1.0f))
                       * glm::vec3(0.0f, 1.0f, 0.0f);

      for (auto i = 0; i < explosionAttack->count; ++i) {
        auto angle = glm::two_pi<float>() / static_cast<float>(explosionAttack->count) * static_cast<float>(i);
        auto newDirection = glm::angleAxis(angle, glm::vec3(0.0f, 0.0f, 1.0f)) * direction;
        _projectileFactory.create(physicalState->currentPosition, glm::vec2(newDirection) * explosionAttack->speed, explosionAttack->power);
      }
    }
  });
}

}  // namespace linguine
