#include "InputTestSystem.h"

namespace linguine {

void InputTestSystem::update(float deltaTime) {
  for (const auto& entry : _inputManager.getTouches()) {
    _logger.log(std::to_string(entry.first) + " -> [ " + std::to_string(entry.second.x) + " : " + std::to_string(entry.second.y) + " ]");
  }
}

}  // namespace linguine
