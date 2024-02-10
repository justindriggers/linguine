#pragma once

#include <glm/vec2.hpp>

namespace linguine {

struct Collapse {
  glm::vec2 startOffset = glm::vec2(0.0f);
  float duration = 1.0f;
  float elapsed = 0.0f;
};

}  // namespace linguine
