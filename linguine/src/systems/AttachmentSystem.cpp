#include "AttachmentSystem.h"

#include <glm/geometric.hpp>
#include <glm/gtx/quaternion.hpp>

#include "components/Attachment.h"
#include "components/PhysicalState.h"

namespace linguine {

void AttachmentSystem::fixedUpdate(float fixedDeltaTime) {
  findEntities<Attachment, PhysicalState>()->each([this](const Entity& entity) {
    auto attachment = entity.get<Attachment>();
    auto physicalState = entity.get<PhysicalState>();

    auto parent = getEntityById(attachment->parentId);
    auto parentPhysicalState = parent->get<PhysicalState>();

    auto offset = glm::angleAxis(parentPhysicalState->currentRotation, glm::vec3(0.0f, 0.0f, 1.0f))
                  * glm::vec3(attachment->offset, 0.0f);

    physicalState->currentPosition = parentPhysicalState->currentPosition + glm::vec2(offset);
    physicalState->currentRotation = parentPhysicalState->currentRotation;
  });
}

}  // namespace linguine
