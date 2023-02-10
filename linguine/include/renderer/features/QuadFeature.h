#pragma once

#include "RenderFeature.h"

#include <glm/mat4x4.hpp>

namespace linguine {

struct QuadFeature : public RenderFeature {
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  float value = 0.0f;
};

}  // namespace linguine
