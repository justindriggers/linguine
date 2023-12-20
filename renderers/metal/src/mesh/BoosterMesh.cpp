#include "BoosterMesh.h"

#include <simd/simd.h>

namespace linguine::render {

BoosterMesh::BoosterMesh(MTL::Device& device) {
  simd::float2 positions[] = {
      // Left Booster A
      { -0.2f, -0.2f },
      { -0.05f, -0.2f },
      { -0.05f, -0.3f },

      // Left Booster B
      { -0.05f, -0.2f },
      { -0.2f, -0.2f },
      { -0.2f, -0.15f },

      // Right Booster A
      { 0.05f, -0.2f },
      { 0.2f, -0.2f },
      { 0.05f, -0.3f },

      // Right Booster B
      { 0.2f, -0.2f },
      { 0.05f, -0.2f },
      { 0.2f, -0.15f }
  };

  const auto positionsBufferSize = std::size(positions) * sizeof(simd::float2);
  _vertexPositionsBuffer = device.newBuffer(positionsBufferSize, MTL::ResourceStorageModeShared);

  memcpy(_vertexPositionsBuffer->contents(), positions, positionsBufferSize);
}

BoosterMesh::~BoosterMesh() {
  _vertexPositionsBuffer->release();
}

void BoosterMesh::bind(MTL::RenderCommandEncoder& encoder) {
  encoder.setVertexBuffer(_vertexPositionsBuffer, 0, 0);
}

void BoosterMesh::draw(MTL::RenderCommandEncoder& encoder) {
  encoder.drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(12));
}

}  // namespace linguine::render
