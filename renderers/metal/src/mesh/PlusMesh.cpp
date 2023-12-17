#include "PlusMesh.h"

#include <simd/simd.h>

namespace linguine::render {

PlusMesh::PlusMesh(MTL::Device& device) {
  simd::float2 positions[] = {
      { -0.25f, -0.25f },
      { -0.25f, 0.25f },
      { 0.25f, 0.25f },

      { -0.25f, -0.25f },
      { 0.25f, 0.25f },
      { 0.25f, -0.25f },

      { -0.5f, -0.25f },
      { -0.5f,  0.25f },
      { -0.25f, 0.25f },

      { -0.5f, -0.25f },
      { -0.25f, 0.25f },
      { -0.25f, -0.25f },

      { -0.25f, 0.25f },
      { -0.25f, 0.5f },
      { 0.25f, 0.5f },

      { -0.25f, 0.25f },
      { 0.25f, 0.5f },
      { 0.25f, 0.25f },

      { 0.25f, -0.25f },
      { 0.25f, 0.25f },
      { 0.5f, 0.25f },

      { 0.25f, -0.25f },
      { 0.5f, 0.25f },
      { 0.5f, -0.25f },

      { -0.25f, -0.5f },
      { -0.25f, -0.25f },
      { 0.25f, -0.25f },

      { -0.25f, -0.5f },
      { 0.25f, -0.25f },
      { 0.25f, -0.5f }
  };

  const auto positionsBufferSize = std::size(positions) * sizeof(simd::float2);
  _vertexPositionsBuffer = device.newBuffer(positionsBufferSize, MTL::ResourceStorageModeShared);

  memcpy(_vertexPositionsBuffer->contents(), positions, positionsBufferSize);
}

PlusMesh::~PlusMesh() {
  _vertexPositionsBuffer->release();
}

void PlusMesh::bind(MTL::RenderCommandEncoder& encoder) {
  encoder.setVertexBuffer(_vertexPositionsBuffer, 0, 0);
}

void PlusMesh::draw(MTL::RenderCommandEncoder& encoder) {
  encoder.drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(30));
}

}  // namespace linguine::render
