#include "AttachmentSystem.h"

#include <glm/geometric.hpp>
#include <glm/gtx/quaternion.hpp>

#include "components/Attachment.h"
#include "components/PhysicalState.h"
#include "components/Transform.h"

namespace linguine {

void AttachmentSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Attachment, PhysicalState>()->each([this](const Entity& entity) {
    auto attachment = entity.get<Attachment>();

    if (attachment->useFixedUpdate) {
      auto physicalState = entity.get<PhysicalState>();

      auto parent = getEntityById(attachment->parentId);
      auto parentPhysicalState = parent->get<PhysicalState>();

      auto offset = glm::angleAxis(parentPhysicalState->currentRotation, glm::vec3(0.0f, 0.0f, 1.0f))
                    * glm::vec3(attachment->offset, 0.0f);

      physicalState->previousPosition = parentPhysicalState->previousPosition + glm::vec2(offset);
      physicalState->currentPosition = parentPhysicalState->currentPosition + glm::vec2(offset);

      physicalState->previousRotation = parentPhysicalState->previousRotation;
      physicalState->currentRotation = parentPhysicalState->currentRotation;
    }
  });
}

void AttachmentSystem::update(float deltaTime) {
  findEntities<Attachment, Transform>()->each([this](const Entity& entity) {
    auto attachment = entity.get<Attachment>();

    if (!attachment->useFixedUpdate) {
      auto transform = entity.get<Transform>();

      auto parent = getEntityById(attachment->parentId);
      auto parentTransform = parent->get<Transform>();

      auto offset = parentTransform->rotation * glm::vec3(attachment->offset, 0.0f);

      auto z = transform->position.z;
      transform->position = parentTransform->position + offset;
      transform->position.z = z;

      transform->rotation = parentTransform->rotation;
    }
  });
}

}  // namespace linguine
