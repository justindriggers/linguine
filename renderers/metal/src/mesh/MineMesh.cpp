#include "MineMesh.h"

#include <simd/simd.h>

#include <glm/gtc/quaternion.hpp>

namespace linguine::render {

MineMesh::MineMesh(MTL::Device& device) {
  auto positions = std::vector<simd::float2>(16 * 3 + 8 * 3);

  for (auto i = 0; i < 16; ++i) {
    auto a = glm::angleAxis(glm::pi<float>() / 16.0f * static_cast<float>(i * 2 + 1), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec3(0.4f, 0.0f, 0.0f);
    auto b = glm::angleAxis(glm::pi<float>() / 16.0f * static_cast<float>(i * 2 + 3), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec3(0.4f, 0.0f, 0.0f);

    positions[i * 3] = { 0.0f, 0.0f };
    positions[i * 3 + 1] = { a.x, a.y };
    positions[i * 3 + 2] = { b.x, b.y };
  }

  for (auto i = 0; i < 8; ++i) {
    auto a = glm::angleAxis(glm::pi<float>() / 16.0f * static_cast<float>(i * 4 + 1), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec3(0.4f, 0.0f, 0.0f);
    auto b = glm::angleAxis(glm::pi<float>() / 16.0f * static_cast<float>(i * 4 - 1), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec3(0.4f, 0.0f, 0.0f);
    auto c = glm::angleAxis(glm::pi<float>() / 8.0f * static_cast<float>(i * 2), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec3(0.5f, 0.0f, 0.0f);

    positions[16 * 3 + i * 3] = { a.x, a.y };
    positions[16 * 3 + i * 3 + 1] = { b.x, b.y };
    positions[16 * 3 + i * 3 + 2] = { c.x, c.y };
  }

  const auto positionsBufferSize = positions.size() * sizeof(simd::float2);
  _vertexPositionsBuffer = device.newBuffer(positionsBufferSize, MTL::ResourceStorageModeShared);

  memcpy(_vertexPositionsBuffer->contents(), positions.data(), positionsBufferSize);
}

MineMesh::~MineMesh() {
  _vertexPositionsBuffer->release();
}

void MineMesh::bind(MTL::RenderCommandEncoder& encoder) {
  encoder.setVertexBuffer(_vertexPositionsBuffer, 0, 0);
}

void MineMesh::draw(MTL::RenderCommandEncoder& encoder) {
  encoder.drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(16 * 3 + 8 * 3));
}

}  // namespace linguine::render
