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
      transform->position = toast->startPosition;
      transform->position.x += -(static_cast<float>(text->feature->text.size()) / 2.0f - 0.5f) * transform->scale.x;
      transform->position.y += glm::lerp(0.0f, toast->distance, lerp);

      text->renderable->setEnabled(true);
    } else {
      text->renderable->setEnabled(false);
    }
  });
}

}  // namespace linguine
