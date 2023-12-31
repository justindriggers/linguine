#pragma once

#include <glm/vec3.hpp>

namespace linguine {

struct Toast {
  glm::vec3 startPosition = glm::vec3(0.0f);
  float duration = 1.0f;
  float elapsed = 1.0f;
};

}  // namespace linguine
