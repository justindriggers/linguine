#include "ShipMesh.h"

#include <simd/simd.h>

namespace linguine::render {

ShipMesh::ShipMesh(MTL::Device& device) {
  simd::float2 positions[] = {
      // Body
      { -0.2f, -0.15f },
      { 0.0f, 0.5f },
      { 0.2f, -0.15f },

      // Left Booster
      { -0.2f, -0.15f },
      { -0.05f, -0.15f },
      { -0.05f, -0.2f },

      // Right Booster
      { 0.2f, -0.15f },
      { 0.05f, -0.2f },
      { 0.05f, -0.15f },

      // Middle A
      { -0.05f, -0.2f },
      { -0.05f, -0.15f },
      { 0.05f, -0.2f },

      // Middle B
      { -0.05f, -0.15f },
      { 0.05f, -0.15f },
      { 0.05f, -0.2f }
  };

  const auto positionsBufferSize = std::size(positions) * sizeof(simd::float2);
  _vertexPositionsBuffer = device.newBuffer(positionsBufferSize, MTL::ResourceStorageModeShared);

  memcpy(_vertexPositionsBuffer->contents(), positions, positionsBufferSize);
}

ShipMesh::~ShipMesh() {
  _vertexPositionsBuffer->release();
}

void ShipMesh::bind(MTL::RenderCommandEncoder& encoder) {
  encoder.setVertexBuffer(_vertexPositionsBuffer, 0, 0);
}

void ShipMesh::draw(MTL::RenderCommandEncoder& encoder) {
  encoder.drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(15));
}

}  // namespace linguine::render
