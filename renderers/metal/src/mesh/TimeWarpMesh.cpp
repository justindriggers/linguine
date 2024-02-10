#include "TimeWarpMesh.h"

#include <simd/simd.h>

namespace linguine::render {

TimeWarpMesh::TimeWarpMesh(MTL::Device& device) {
  simd::float2 positions[] = {
      // Base
      { -0.375f, -0.475f },
      { -0.375f, -0.325f },
      {  0.375f, -0.325f },

      {  0.375f, -0.325f },
      {  0.375f, -0.475f },
      { -0.375f, -0.475f },

      // Cap
      { -0.375f,  0.325f },
      { -0.375f,  0.475f },
      {  0.375f,  0.475f },

      {  0.375f,  0.475f },
      {  0.375f,  0.325f },
      { -0.375f,  0.325f },

      // Bottom
      { -0.375f, -0.325f },
      { -0.125f,  0.0f },
      {  0.0f,   -0.325f },

      { -0.125f,  0.0f },
      {  0.125f,  0.0f },
      {  0.0f,   -0.325f },

      {  0.375f, -0.325f },
      {  0.0f,   -0.325f },
      {  0.125f,  0.0f },

      // Top
      { -0.375f,  0.325f },
      {  0.0f,    0.325f },
      { -0.125f,  0.0f },

      { -0.125f,  0.0f },
      {  0.0f,    0.325f },
      {  0.125f,  0.0f },

      {  0.125f,  0.0f },
      {  0.0f,    0.325f },
      {  0.375f,  0.325f }
  };

  const auto positionsBufferSize = std::size(positions) * sizeof(simd::float2);
  _vertexPositionsBuffer = device.newBuffer(positionsBufferSize, MTL::ResourceStorageModeShared);

  memcpy(_vertexPositionsBuffer->contents(), positions, positionsBufferSize);
}

TimeWarpMesh::~TimeWarpMesh() {
  _vertexPositionsBuffer->release();
}

void TimeWarpMesh::bind(MTL::RenderCommandEncoder& encoder) {
  encoder.setVertexBuffer(_vertexPositionsBuffer, 0, 0);
}

void TimeWarpMesh::draw(MTL::RenderCommandEncoder& encoder) {
  encoder.drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(30));
}

}  // namespace linguine::render
