#include "ShipMesh.h"

namespace linguine::render {

ShipMesh::ShipMesh() {
  GLfloat positions[] = {
      // Body
      -0.2f, -0.15f,
      0.2f, -0.15f,
      0.0f, 0.5f,
      
      // Left Booster
      -0.2f, -0.15f,
      -0.05f, -0.2f,
      -0.05f, -0.15f,

      // Right Booster
      0.2f, -0.15f,
      0.05f, -0.15f,
      0.05f, -0.2f,

      // Middle A
      -0.05f, -0.2f,
      0.05f, -0.2f,
      -0.05f, -0.15f,

      // Middle B
      -0.05f, -0.15f,
      0.05f, -0.2f,
      0.05f, -0.15f
  };

  glGenBuffers(1, &_vertexPositionsBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
}

ShipMesh::~ShipMesh() {
  glDeleteBuffers(1, &_vertexPositionsBuffer);
}

void ShipMesh::bind() {
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void ShipMesh::unbind() {
  glDisableVertexAttribArray(0);
}

void ShipMesh::draw() {
  glDrawArrays(GL_TRIANGLES, 0, 15);
}

}  // namespace linguine::render
