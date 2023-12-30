#include "FireSystem.h"

#include "components/Attachment.h"
#include "components/Fire.h"
#include "components/Transform.h"

namespace linguine {

void FireSystem::update(float deltaTime) {
  findEntities<Fire, Transform, Attachment>()->each([this](const Entity& entity) {
    auto transform = entity.get<Transform>();
    auto attachment = entity.get<Attachment>();
    auto parent = getEntityById(attachment->parentId);
    auto parentTransform = parent->get<Transform>();

    auto randomScale = std::uniform_real_distribution(0.1f, 0.15f);
    transform->scale = glm::vec3(randomScale(_random) * parentTransform->scale);
  });
}

}  // namespace linguine
