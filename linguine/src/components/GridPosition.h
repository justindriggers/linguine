#pragma once

#include <optional>

#include <glm/vec2.hpp>

namespace linguine {

struct GridPosition {
  glm::ivec2 dimensions = {1, 1};
  glm::vec2 position;
  std::optional<glm::ivec2> destination;
  float speed = 2.0f;
};

}  // namespace linguine
