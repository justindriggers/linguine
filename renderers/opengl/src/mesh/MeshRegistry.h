#pragma once

#include <memory>
#include <unordered_map>

#include <renderer/mesh/MeshType.h>

#include "Mesh.h"

namespace linguine::render {

class MeshRegistry {
  public:
    MeshRegistry();

    std::unique_ptr<Mesh>& get(MeshType meshType);

  private:
    std::unordered_map<MeshType, std::unique_ptr<Mesh>> _meshes;
};

}  // namespace linguine::render
