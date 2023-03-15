#pragma once

#include <glm/vec2.hpp>

namespace linguine {

struct PhysicalState {
  glm::vec2 previousPosition;
  glm::vec2 currentPosition;
};

}  // namespace linguine
