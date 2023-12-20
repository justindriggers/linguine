#include "CockpitMesh.h"

#include <simd/simd.h>

namespace linguine::render {

CockpitMesh::CockpitMesh(MTL::Device& device) {
  simd::float2 positions[] = {
      { 0.0f, 0.2f },
      { 0.1f, -0.1f },
      { -0.1f, -0.1f },

      { -0.1f, -0.1f },
      { 0.1f, -0.1f },
      { 0.0f, -0.15f }
  };

  const auto positionsBufferSize = std::size(positions) * sizeof(simd::float2);
  _vertexPositionsBuffer = device.newBuffer(positionsBufferSize, MTL::ResourceStorageModeShared);

  memcpy(_vertexPositionsBuffer->contents(), positions, positionsBufferSize);
}

CockpitMesh::~CockpitMesh() {
  _vertexPositionsBuffer->release();
}

void CockpitMesh::bind(MTL::RenderCommandEncoder& encoder) {
  encoder.setVertexBuffer(_vertexPositionsBuffer, 0, 0);
}

void CockpitMesh::draw(MTL::RenderCommandEncoder& encoder) {
  encoder.drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(6));
}

}  // namespace linguine::render
