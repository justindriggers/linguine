#pragma once

#include <glm/vec2.hpp>

namespace linguine {

struct Projectile {
  uint64_t actor;
  uint64_t target;
  float speed = 1.0f;
  int32_t power = 0;
};

}  // namespace linguine
