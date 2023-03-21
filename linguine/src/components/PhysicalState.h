#pragma once

#include <glm/vec2.hpp>

namespace linguine {

struct PhysicalState {
  glm::vec2 previousPosition = glm::vec2(0.0f);
  glm::vec2 currentPosition = glm::vec2(0.0f);
  float previousRotation = 0.0f;
  float currentRotation = 0.0f;
};

}  // namespace linguine
