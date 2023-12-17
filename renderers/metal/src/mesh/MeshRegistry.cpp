#include "MeshRegistry.h"

#include "MineMesh.h"
#include "PlusMesh.h"
#include "QuadMesh.h"
#include "TriangleMesh.h"

namespace linguine::render {

MeshRegistry::MeshRegistry(MTL::Device& device) {
  _meshes.insert({MeshType::Mine, std::make_unique<MineMesh>(device)});
  _meshes.insert({MeshType::Plus, std::make_unique<PlusMesh>(device)});
  _meshes.insert({MeshType::Quad, std::make_unique<QuadMesh>(device)});
  _meshes.insert({MeshType::Triangle, std::make_unique<TriangleMesh>(device)});
}

std::unique_ptr<Mesh>& MeshRegistry::get(MeshType meshType) {
  return _meshes[meshType];
}

}  // namespace linguine::render
