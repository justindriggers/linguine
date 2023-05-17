#include "PlayerAttackSystem.h"

#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/Friendly.h"
#include "components/PhysicalState.h"
#include "components/Player.h"
#include "components/PlayerTarget.h"
#include "components/Projectile.h"
#include "components/ProjectileAttack.h"
#include "components/Transform.h"

namespace linguine {

void PlayerAttackSystem::update(float deltaTime) {
  findEntities<Player, ProjectileAttack, PhysicalState>()->each([this, deltaTime](const Entity& playerEntity) {
    auto projectileAttack = playerEntity.get<ProjectileAttack>();
    projectileAttack->elapsed += deltaTime;

    findEntities<PlayerTarget>()->each([this, &playerEntity, &projectileAttack](const Entity& playerTargetEntity) {
      auto playerTarget = playerTargetEntity.get<PlayerTarget>();

      if (playerTarget->entityId) {
        auto target = getEntityById(*playerTarget->entityId);
        auto targetPhysicalState = target->get<PhysicalState>();
        auto playerPhysicalState = playerEntity.get<PhysicalState>();

        if (projectileAttack->elapsed >= projectileAttack->frequency
            && glm::distance(playerPhysicalState->currentPosition, targetPhysicalState->currentPosition) <= projectileAttack->range) {
          projectileAttack->elapsed = 0.0f;

          auto entity = createEntity();
          entity->add<Friendly>();

          auto projectile = entity->add<Projectile>();
          projectile->actor = playerEntity.getId();
          projectile->speed = projectileAttack->speed;
          projectile->power = projectileAttack->power;
          projectile->target = target->getId();

          auto transform = entity->add<Transform>();
          transform->position = glm::vec3(playerPhysicalState->currentPosition, 2.0f);
          transform->scale = glm::vec3(0.25f);

          auto physicalState = entity->add<PhysicalState>();
          physicalState->previousPosition = glm::vec2(transform->position);
          physicalState->currentPosition = physicalState->previousPosition;

          auto collider = entity->add<CircleCollider>();
          collider->radius = 0.125f;

          auto drawable = entity->add<Drawable>();
          drawable->feature = new ColoredFeature();
          drawable->feature->meshType = Quad;
          drawable->feature->color = glm::vec3(0.0f, 1.0f, 0.0f);
          drawable->renderable = _renderer.create(std::unique_ptr<ColoredFeature>(drawable->feature));
          drawable.setRemovalListener([drawable](const Entity e) {
            drawable->renderable->destroy();
          });
        }
      }
    });
  });
}

}  // namespace linguine
