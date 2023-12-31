#include "ToastSystem.h"

#include <glm/gtx/compatibility.hpp>

#include "components/Text.h"
#include "components/Toast.h"
#include "components/Transform.h"

namespace linguine {

void ToastSystem::update(float deltaTime) {
  findEntities<Toast, Transform, Text>()->each([deltaTime](const Entity& entity) {
    auto toast = entity.get<Toast>();
    toast->elapsed += deltaTime;

    auto text = entity.get<Text>();

    if (toast->elapsed <= toast->duration) {
      float lerp = toast->elapsed / toast->duration;

      auto transform = entity.get<Transform>();
      transform->scale = glm::vec3(6.0f);
      transform->position = toast->startPosition;
      transform->position.x += -static_cast<float>(text->feature->text.size()) * 6.0f / 2.0f + 3.0f;
      transform->position.y += glm::lerp(0.0f, 32.0f, lerp);

      text->renderable->setEnabled(true);
    } else {
      text->renderable->setEnabled(false);
    }
  });
}

}  // namespace linguine
