#include "CollapseSystem.h"

#include <glm/gtx/compatibility.hpp>

#include "components/Attachment.h"
#include "components/Collapse.h"

namespace linguine {

void CollapseSystem::update(float deltaTime) {
  findEntities<Collapse, Attachment>()->each([deltaTime](const Entity& entity) {
    auto collapse = entity.get<Collapse>();
    collapse->elapsed += deltaTime;

    auto lerp = collapse->elapsed / collapse->duration;

    auto attachment = entity.get<Attachment>();
    attachment->offset = glm::lerp(collapse->startOffset, glm::vec2(), lerp);
  });
}

}  // namespace linguine
