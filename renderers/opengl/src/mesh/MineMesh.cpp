#include "MineMesh.h"

#include <vector>

#include <glm/gtc/quaternion.hpp>

namespace linguine::render {

MineMesh::MineMesh() {
  auto positions = std::vector<GLfloat>(24 * 3 * 2);

  for (auto i = 0; i < 16; ++i) {
    auto a = glm::angleAxis(glm::pi<float>() / 16.0f * static_cast<float>(i * 2 + 1), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec3(0.4f, 0.0f, 0.0f);
    auto b = glm::angleAxis(glm::pi<float>() / 16.0f * static_cast<float>(i * 2 + 3), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec3(0.4f, 0.0f, 0.0f);

    positions[i * 6] = 0.0f;
    positions[i * 6 + 1] = 0.0f;

    positions[i * 6 + 2] = a.x;
    positions[i * 6 + 3] = a.y;

    positions[i * 6 + 4] = b.x;
    positions[i * 6 + 5] = b.y;
  }

  for (auto i = 0; i < 8; ++i) {
    auto a = glm::angleAxis(glm::pi<float>() / 16.0f * static_cast<float>(i * 4 + 1), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec3(0.4f, 0.0f, 0.0f);
    auto b = glm::angleAxis(glm::pi<float>() / 16.0f * static_cast<float>(i * 4 - 1), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec3(0.4f, 0.0f, 0.0f);
    auto c = glm::angleAxis(glm::pi<float>() / 8.0f * static_cast<float>(i * 2), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec3(0.5f, 0.0f, 0.0f);

    positions[16 * 3 * 2 + i * 6] = a.x;
    positions[16 * 3 * 2 + i * 6 + 1] = a.y;

    positions[16 * 3 * 2 + i * 6 + 2] = b.x;
    positions[16 * 3 * 2 + i * 6 + 3] = b.y;

    positions[16 * 3 * 2 + i * 6 + 4] = c.x;
    positions[16 * 3 * 2 + i * 6 + 5] = c.y;
  }

  glGenBuffers(1, &_vertexPositionsBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GLfloat), positions.data(), GL_STATIC_DRAW);
}

MineMesh::~MineMesh() {
  glDeleteBuffers(1, &_vertexPositionsBuffer);
}

void MineMesh::bind() {
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void MineMesh::unbind() {
  glDisableVertexAttribArray(0);
}

void MineMesh::draw() {
  glDrawArrays(GL_TRIANGLES, 0, 24 * 3);
}

}  // namespace linguine::render
