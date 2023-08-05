#pragma once

#include "RenderFeature.h"

#include <string>

#include <glm/mat4x4.hpp>

namespace linguine {

struct TextFeature : public RenderFeature {
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  glm::vec3 color = glm::vec3(1.0f);
  std::string text;
};

}  // namespace linguine
