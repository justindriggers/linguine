#include "RotatorSystem.h"

#include "components/Rotating.h"
#include "components/Transform.h"

namespace linguine {

void RotatorSystem::update(float deltaTime) {
  findEntities<Rotating, Transform>()->each([deltaTime](const Entity& entity) {
    const auto rotating = entity.get<Rotating>();
    const auto transform = entity.get<Transform>();

    transform->rotation *= glm::angleAxis(glm::two_pi<float>() * rotating->speed * deltaTime, glm::vec3(0.0f, 0.0f, 1.0f));
  });
}

}  // namespace linguine