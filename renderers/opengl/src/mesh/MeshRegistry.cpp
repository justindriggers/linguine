#include "MeshRegistry.h"

#include "MineMesh.h"
#include "PlusMesh.h"
#include "QuadMesh.h"
#include "TriangleMesh.h"

namespace linguine::render {

MeshRegistry::MeshRegistry() {
  _meshes.insert({MeshType::Mine, std::make_unique<MineMesh>()});
  _meshes.insert({MeshType::Plus, std::make_unique<PlusMesh>()});
  _meshes.insert({MeshType::Quad, std::make_unique<QuadMesh>()});
  _meshes.insert({MeshType::Triangle, std::make_unique<TriangleMesh>()});
}

std::unique_ptr<Mesh>& MeshRegistry::get(MeshType meshType) {
  return _meshes[meshType];
}

}  // namespace linguine::render
