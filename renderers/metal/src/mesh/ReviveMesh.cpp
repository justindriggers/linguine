#include "ReviveMesh.h"

#include <simd/simd.h>

namespace linguine::render {

ReviveMesh::ReviveMesh(MTL::Device& device) {
  simd::float2 positions[] = {
      {  0.125f,  0.075f },
      { -0.25f,   0.075f },
      { -0.125f,  0.5f },

      { -0.125f,  0.5f },
      {  0.25f,   0.5f },
      {  0.125f,  0.075f },

      { -0.125f,  0.075f },
      {  0.25f,   0.075f },
      { -0.25f,  -0.5f }
  };

  const auto positionsBufferSize = std::size(positions) * sizeof(simd::float2);
  _vertexPositionsBuffer = device.newBuffer(positionsBufferSize, MTL::ResourceStorageModeShared);

  memcpy(_vertexPositionsBuffer->contents(), positions, positionsBufferSize);
}

ReviveMesh::~ReviveMesh() {
  _vertexPositionsBuffer->release();
}

void ReviveMesh::bind(MTL::RenderCommandEncoder& encoder) {
  encoder.setVertexBuffer(_vertexPositionsBuffer, 0, 0);
}

void ReviveMesh::draw(MTL::RenderCommandEncoder& encoder) {
  encoder.drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(9));
}

}  // namespace linguine::render
