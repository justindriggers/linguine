#pragma once

#include <optional>

#include <glm/vec2.hpp>

#include "data/RaycastHit.h"

namespace linguine {

struct Raycaster {
  glm::vec2 direction = { 0.0f, 1.0f };
  float distance = 50.0f;
  std::optional<RaycastHit> nearest = {};
};

}  // namespace linguine
