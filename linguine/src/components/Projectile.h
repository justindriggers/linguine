#pragma once

#include <glm/vec2.hpp>

namespace linguine {

struct Projectile {
  glm::vec2 velocity = glm::vec2(0.0f, 1.0f);
  int32_t power = 0;
};

}  // namespace linguine
