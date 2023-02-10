#pragma once

#include <glm/mat4x4.hpp>

namespace linguine {

struct Camera {
  glm::mat4 viewMatrix = glm::mat4(1.0f);
};

}  // namespace linguine
