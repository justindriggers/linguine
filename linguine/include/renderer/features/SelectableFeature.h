#pragma once

#include "RenderFeature.h"

#include <glm/mat4x4.hpp>

#include "renderer/mesh/MeshType.h"

namespace linguine {

struct SelectableFeature : public RenderFeature {
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  uint64_t entityId;
  MeshType meshType = MeshType::Quad;
};

}  // namespace linguine
