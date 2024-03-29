#include "QuadMesh.h"

#include <simd/simd.h>

namespace linguine::render {

QuadMesh::QuadMesh(MTL::Device& device) {
  simd::float2 positions[] = {
      { -0.5f, -0.5f },
      { -0.5f,  0.5f },
      {  0.5f, -0.5f },

      {  0.5f, -0.5f },
      { -0.5f,  0.5f },
      {  0.5f,  0.5f }
  };

  const auto positionsBufferSize = std::size(positions) * sizeof(simd::float2);
  _vertexPositionsBuffer = device.newBuffer(positionsBufferSize, MTL::ResourceStorageModeShared);

  memcpy(_vertexPositionsBuffer->contents(), positions, positionsBufferSize);
}

QuadMesh::~QuadMesh() {
  _vertexPositionsBuffer->release();
}

void QuadMesh::bind(MTL::RenderCommandEncoder& encoder) {
  encoder.setVertexBuffer(_vertexPositionsBuffer, 0, 0);
}

void QuadMesh::draw(MTL::RenderCommandEncoder& encoder) {
  encoder.drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(6));
}

}  // namespace linguine::render
