#include "MeshRegistry.h"

#include "QuadMesh.h"
#include "TriangleMesh.h"

namespace linguine::render {

MeshRegistry::MeshRegistry() {
  _meshes.insert({MeshType::Quad, std::make_unique<QuadMesh>()});
  _meshes.insert({MeshType::Triangle, std::make_unique<TriangleMesh>()});
}

std::unique_ptr<Mesh>& MeshRegistry::get(MeshType meshType) {
  return _meshes[meshType];
}

}  // namespace linguine::render
