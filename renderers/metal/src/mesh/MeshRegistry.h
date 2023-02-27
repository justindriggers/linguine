#pragma once

#include "Metal/Metal.hpp"

#include <memory>
#include <unordered_map>

#include <renderer/mesh/MeshType.h>

#include "Mesh.h"

namespace linguine::render {

class MeshRegistry {
  public:
    explicit MeshRegistry(MTL::Device& device);

    std::unique_ptr<Mesh>& get(MeshType meshType);

  private:
    std::unordered_map<MeshType, std::unique_ptr<Mesh>> _meshes;
};

}  // namespace linguine::render
