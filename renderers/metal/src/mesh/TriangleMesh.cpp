#include "TriangleMesh.h"

#include <simd/simd.h>

namespace linguine::render {

TriangleMesh::TriangleMesh(MTL::Device& device) {
  simd::float2 positions[] = {
      { -0.5f, -0.5f },
      {  0.0f,  0.5f },
      {  0.5f, -0.5f }
  };

  const auto positionsBufferSize = std::size(positions) * sizeof(simd::float2);
  _vertexPositionsBuffer = device.newBuffer(positionsBufferSize, MTL::ResourceStorageModeShared);

  memcpy(_vertexPositionsBuffer->contents(), positions, positionsBufferSize);
}

TriangleMesh::~TriangleMesh() {
  _vertexPositionsBuffer->release();
}

void TriangleMesh::bind(MTL::RenderCommandEncoder& encoder) {
  encoder.setVertexBuffer(_vertexPositionsBuffer, 0, 0);
}

void TriangleMesh::draw(MTL::RenderCommandEncoder& encoder) {
  encoder.drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
}

}  // namespace linguine::render
