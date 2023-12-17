#pragma once

#include "RenderFeature.h"

#include <glm/mat4x4.hpp>

#include "renderer/mesh/MeshType.h"

namespace linguine {

struct ColoredFeature : public RenderFeature {
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  glm::vec3 color = glm::vec3(1.0f);
  MeshType meshType = Quad;
};

}  // namespace linguine