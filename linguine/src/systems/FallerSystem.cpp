#include "FallerSystem.h"

#include "components/Falling.h"
#include "components/Transform.h"

namespace linguine {

void FallerSystem::update(float deltaTime) {
  findEntities<Falling, Transform>()->each([deltaTime](const Entity& entity) {
    const auto falling = entity.get<Falling>();
    const auto transform = entity.get<Transform>();

    transform->position -= glm::vec3(0.0f, 1.0f, 0.0f) * falling->speed * deltaTime;
  });
}

}  // namespace linguine