#include "QuadTransformationSystem.h"

#include "components/Transform.h"
#include "components/Quad.h"

namespace linguine {

void QuadTransformationSystem::update(float deltaTime) {
  findEntities<Transform, Quad>()->each([](const Entity& entity) {
    const auto transform = entity.get<Transform>();
    const auto quad = entity.get<Quad>();

    quad->feature->modelMatrix = glm::translate(glm::mat4(1.0f), transform->position)
                                 * glm::mat4_cast(transform->rotation);
  });
}

}  // namespace linguine
