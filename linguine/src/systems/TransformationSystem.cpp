#include "TransformationSystem.h"

#include "components/Drawable.h"
#include "components/Selectable.h"
#include "components/Transform.h"

namespace linguine {

void TransformationSystem::update(float deltaTime) {
  findEntities<Transform, Drawable>()->each([](const Entity& entity) {
    const auto transform = entity.get<Transform>();
    const auto drawable = entity.get<Drawable>();

    drawable->feature->modelMatrix = glm::translate(glm::mat4(1.0f), transform->position)
                                     * glm::mat4_cast(transform->rotation);
  });

  findEntities<Transform, Selectable>()->each([](const Entity& entity) {
    const auto transform = entity.get<Transform>();
    const auto selectable = entity.get<Selectable>();

    selectable->feature->modelMatrix = glm::translate(glm::mat4(1.0f), transform->position)
                                       * glm::mat4_cast(transform->rotation);
  });
}

}  // namespace linguine
