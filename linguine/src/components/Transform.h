#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace linguine {

struct Transform {
  glm::vec3 position = glm::vec3(0.0f);
  glm::quat rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
};

}  // namespace linguine
