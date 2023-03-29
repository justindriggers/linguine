#include "EnemyAttackSystem.h"

#include <glm/common.hpp>

#include "components/Alive.h"
#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/Hostile.h"
#include "components/PhysicalState.h"
#include "components/Projectile.h"
#include "components/Targeting.h"
#include "components/Transform.h"
#include "components/Unit.h"

namespace linguine {

void EnemyAttackSystem::update(float deltaTime) {
  findEntities<Hostile, Unit, Alive, PhysicalState, Targeting>()->each([this, deltaTime](const Entity& entity) {
    auto unit = entity.get<Unit>();
    auto position = entity.get<PhysicalState>()->currentPosition;

    if (unit->attackTimer >= unit->attackSpeed) {
      unit->attackTimer -= unit->attackSpeed;

      auto targeting = entity.get<Targeting>();

      if (targeting->current) {
        auto target = getEntityById(*targeting->current);
        auto targetPosition = target->get<PhysicalState>()->currentPosition;

        auto direction = glm::normalize(glm::vec2(targetPosition) - glm::vec2(position));
        auto velocity = direction;

        createProjectile(position, velocity, unit->attackPower);
      }
    }

    unit->attackTimer += deltaTime;
  });
}

void EnemyAttackSystem::createProjectile(glm::vec2 location, glm::vec2 velocity, int32_t power) {
  auto entity = createEntity();
  entity->add<Hostile>();

  auto projectile = entity->add<Projectile>();
  projectile->velocity = velocity;
  projectile->power = power;

  auto transform = entity->add<Transform>();
  transform->position = glm::vec3(location, 2.0f);
  transform->scale = glm::vec3(0.25f);

  auto physicalState = entity->add<PhysicalState>();
  physicalState->previousPosition = glm::vec2(transform->position);
  physicalState->currentPosition = physicalState->previousPosition;

  auto collider = entity->add<CircleCollider>();
  collider->radius = 0.125f;

  auto drawable = entity->add<Drawable>();
  drawable->feature = new ColoredFeature();
  drawable->feature->meshType = Quad;
  drawable->feature->color = glm::vec3(1.0f, 0.0f, 0.0f);
  drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
  drawable.setRemovalListener([drawable](const Entity e) {
    drawable->renderable->destroy();
  });
}

}  // namespace linguine
