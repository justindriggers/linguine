#pragma once

#include <glm/mat4x4.hpp>

namespace linguine {

struct Camera {
  glm::mat4 viewMatrix = glm::mat4(1.0f);
  glm::mat4 projectionMatrix = glm::mat4(1.0f);
  glm::mat4 viewProjectionMatrix = glm::mat4(1.0f);
  glm::vec3 clearColor = glm::vec3(0.0f);
};

}  // namespace linguine
