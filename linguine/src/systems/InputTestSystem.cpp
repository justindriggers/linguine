#include "InputTestSystem.h"

#include "components/Drawable.h"

namespace linguine {

void InputTestSystem::update(float deltaTime) {
  for (const auto& entry : _inputManager.getTouches()) {
    auto id = _renderer.getEntityIdAt(entry.second.x, entry.second.y);

    if (id.has_value()) {
      auto entity = getEntityById(id.value());
      auto drawable = entity->get<Drawable>();
      drawable->feature->color = glm::vec3(1.0f, 0.0f, 0.0f);
    } else {
      _logger.log("No entity found");
    }
  }
}

}  // namespace linguine
