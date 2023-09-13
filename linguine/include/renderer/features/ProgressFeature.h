#pragma once

#include "RenderFeature.h"

#include <glm/mat4x4.hpp>

#include "renderer/mesh/MeshType.h"

namespace linguine {

struct ProgressFeature : public RenderFeature {
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  glm::vec3 color = glm::vec3(1.0f);
  glm::vec3 backgroundColor = glm::vec3(0.0f);
  float progress = 1.0f;
  MeshType meshType;
};

}  // namespace linguine
