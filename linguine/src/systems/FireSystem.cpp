#include "FireSystem.h"

#include "components/Fire.h"
#include "components/Transform.h"

namespace linguine {

void FireSystem::update(float deltaTime) {
  findEntities<Fire, Transform>()->each([this](const Entity& entity) {
    auto transform = entity.get<Transform>();

    auto randomScale = std::uniform_real_distribution(0.2f, 0.3f);
    transform->scale = glm::vec3(randomScale(_random));
  });
}

}  // namespace linguine
