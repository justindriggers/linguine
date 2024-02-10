#include "SpeedBoostMesh.h"

#include <simd/simd.h>

namespace linguine::render {

SpeedBoostMesh::SpeedBoostMesh(MTL::Device& device) {
  simd::float2 positions[] = {
      // Bottom Bottom-Left
      { -0.425f, -0.5f },
      { -0.425f, -0.25f },
      {  0.0f,   -0.25f },

      // Bottom Bottom-Right
      {  0.425f, -0.5f },
      {  0.0f,   -0.25f },
      {  0.425f, -0.25f },

      // Bottom Center
      { -0.425f, -0.25f },
      {  0.0f,    0.0f },
      {  0.425f, -0.25f },

      // Top Bottom-Left
      { -0.425f,  0.0f },
      { -0.425f,  0.25f },
      {  0.0f,    0.25f },

      // Top Bottom-Right
      {  0.425f,  0.0f },
      {  0.0f,    0.25f },
      {  0.425f,  0.25f },

      // Top Center
      { -0.425f,  0.25f },
      {  0.0f,    0.5f },
      {  0.425f,  0.25f }
  };

  const auto positionsBufferSize = std::size(positions) * sizeof(simd::float2);
  _vertexPositionsBuffer = device.newBuffer(positionsBufferSize, MTL::ResourceStorageModeShared);

  memcpy(_vertexPositionsBuffer->contents(), positions, positionsBufferSize);
}

SpeedBoostMesh::~SpeedBoostMesh() {
  _vertexPositionsBuffer->release();
}

void SpeedBoostMesh::bind(MTL::RenderCommandEncoder& encoder) {
  encoder.setVertexBuffer(_vertexPositionsBuffer, 0, 0);
}

void SpeedBoostMesh::draw(MTL::RenderCommandEncoder& encoder) {
  encoder.drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(18));
}

}  // namespace linguine::render
