#include "EnemyAttackSystem.h"

#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/Friendly.h"
#include "components/Health.h"
#include "components/Hit.h"
#include "components/Hostile.h"
#include "components/MeleeAttack.h"
#include "components/PhysicalState.h"
#include "components/Projectile.h"
#include "components/ProjectileAttack.h"
#include "components/Targeting.h"
#include "components/Transform.h"
#include "components/Trigger.h"

namespace linguine {

void EnemyAttackSystem::update(float deltaTime) {
  findEntities<Hostile, ProjectileAttack, Targeting, PhysicalState>()->each([this, deltaTime](const Entity& enemyEntity) {
    auto targeting = enemyEntity.get<Targeting>();
    auto enemyPhysicalState = enemyEntity.get<PhysicalState>();
    auto projectileAttack = enemyEntity.get<ProjectileAttack>();
    projectileAttack->elapsed += deltaTime;

    if (targeting->current) {
      auto target = getEntityById(*targeting->current);
      auto targetPhysicalState = target->get<PhysicalState>();

      if (projectileAttack->elapsed >= projectileAttack->frequency
            && glm::distance(enemyPhysicalState->currentPosition, targetPhysicalState->currentPosition) <= projectileAttack->range) {
        projectileAttack->elapsed = 0.0f;

        auto entity = createEntity();
        entity->add<Hostile>();

        auto projectile = entity->add<Projectile>();
        projectile->actor = enemyEntity.getId();
        projectile->speed = projectileAttack->speed;
        projectile->power = projectileAttack->power;
        projectile->target = *targeting->current;

        auto transform = entity->add<Transform>();
        transform->position = glm::vec3(enemyPhysicalState->currentPosition, 2.0f);
        transform->scale = glm::vec3(0.25f);

        auto physicalState = entity->add<PhysicalState>();
        physicalState->previousPosition = glm::vec2(transform->position);
        physicalState->currentPosition = physicalState->previousPosition;

        auto collider = entity->add<CircleCollider>();
        collider->radius = 0.125f;

        entity->add<Trigger>();

        auto drawable = entity->add<Drawable>();
        drawable->feature = new ColoredFeature();
        drawable->feature->meshType = Quad;
        drawable->feature->color = glm::vec3(1.0f, 0.0f, 0.0f);
        drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
        drawable.setRemovalListener([drawable](const Entity e) {
          drawable->renderable->destroy();
        });
      }
    }
  });
}

void EnemyAttackSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Hostile, MeleeAttack, Targeting, Hit>()->each([this](const Entity& entity) {
    auto targeting = entity.get<Targeting>();
    auto hit = entity.get<Hit>();

    if (targeting->current) {
      for (const auto entityId : hit->entityIds) {
        if (entityId == *targeting->current) {
          auto meleeAttack = entity.get<MeleeAttack>();

          auto target = getEntityById(*targeting->current);
          auto targetPosition = target->get<PhysicalState>()->currentPosition;

          auto friendlies = findEntities<Friendly, Health>()->get();

          if (friendlies.empty()) {
            if (target->has<Health>()) {
              auto health = target->get<Health>();
              health->current = glm::clamp<int32_t>(health->current - meleeAttack->power, 0, health->max);
            }
          } else {
            auto randomEntity = std::uniform_int_distribution<>(0, static_cast<int>(friendlies.size() - 1));
            const auto targetIndex = randomEntity(_random);
            auto health = friendlies[targetIndex]->get<Health>();
            health->current = glm::clamp<int32_t>(health->current - meleeAttack->power, 0, health->max);
          }
        }
      }
    }
  });
}

}  // namespace linguine