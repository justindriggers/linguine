#include "WingMesh.h"

#include <simd/simd.h>

namespace linguine::render {

WingMesh::WingMesh(MTL::Device& device) {
  simd::float2 positions[] = {
      // Left Wing
      { -0.4f, -0.1f },
      { -0.2f, 0.2f },
      { -0.2f, -0.15f },

      // Body A
      { -0.2f, -0.15f },
      { -0.2f, 0.2f },
      { 0.2f, 0.2f },

      // Body B
      { -0.2f, -0.15f },
      { 0.2f, 0.2f },
      { 0.2f, -0.15f },

      // Right Wing
      { 0.4f, -0.1f },
      { 0.2f, -0.15f },
      { 0.2f, 0.2f }
  };

  const auto positionsBufferSize = std::size(positions) * sizeof(simd::float2);
  _vertexPositionsBuffer = device.newBuffer(positionsBufferSize, MTL::ResourceStorageModeShared);

  memcpy(_vertexPositionsBuffer->contents(), positions, positionsBufferSize);
}

WingMesh::~WingMesh() {
  _vertexPositionsBuffer->release();
}

void WingMesh::bind(MTL::RenderCommandEncoder& encoder) {
  encoder.setVertexBuffer(_vertexPositionsBuffer, 0, 0);
}

void WingMesh::draw(MTL::RenderCommandEncoder& encoder) {
  encoder.drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(12));
}

}  // namespace linguine::render
