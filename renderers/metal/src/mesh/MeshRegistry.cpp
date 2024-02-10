#include "MeshRegistry.h"

#include "BoosterMesh.h"
#include "CockpitMesh.h"
#include "MineMesh.h"
#include "PlusMesh.h"
#include "QuadMesh.h"
#include "ReviveMesh.h"
#include "ShipMesh.h"
#include "SpeedBoostMesh.h"
#include "TimeWarpMesh.h"
#include "TriangleMesh.h"
#include "WingMesh.h"

namespace linguine::render {

MeshRegistry::MeshRegistry(MTL::Device& device) {
  _meshes.insert({MeshType::Booster, std::make_unique<BoosterMesh>(device)});
  _meshes.insert({MeshType::Cockpit, std::make_unique<CockpitMesh>(device)});
  _meshes.insert({MeshType::Mine, std::make_unique<MineMesh>(device)});
  _meshes.insert({MeshType::Plus, std::make_unique<PlusMesh>(device)});
  _meshes.insert({MeshType::Quad, std::make_unique<QuadMesh>(device)});
  _meshes.insert({MeshType::Revive, std::make_unique<ReviveMesh>(device)});
  _meshes.insert({MeshType::Ship, std::make_unique<ShipMesh>(device)});
  _meshes.insert({MeshType::SpeedBoost, std::make_unique<SpeedBoostMesh>(device)});
  _meshes.insert({MeshType::TimeWarp, std::make_unique<TimeWarpMesh>(device)});
  _meshes.insert({MeshType::Triangle, std::make_unique<TriangleMesh>(device)});
  _meshes.insert({MeshType::Wing, std::make_unique<WingMesh>(device)});
}

std::unique_ptr<Mesh>& MeshRegistry::get(MeshType meshType) {
  return _meshes[meshType];
}

}  // namespace linguine::render
