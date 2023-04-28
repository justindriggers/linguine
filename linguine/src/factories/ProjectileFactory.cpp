#include "ProjectileFactory.h"

#include "components/CircleCollider.h"
#include "components/Drawable.h"
#include "components/Hostile.h"
#include "components/PhysicalState.h"
#include "components/Projectile.h"
#include "components/Transform.h"
#include "entity/Entity.h"

namespace linguine {

void ProjectileFactory::create(glm::vec2 location, float speed, int32_t power) {
  auto entity = _entityManager.create();
  entity->add<Hostile>();

  auto projectile = entity->add<Projectile>();
  projectile->speed = speed;
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
