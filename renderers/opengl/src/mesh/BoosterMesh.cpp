#include "BoosterMesh.h"

namespace linguine::render {

BoosterMesh::BoosterMesh() {
  GLfloat positions[] = {
      // Left Booster A
      -0.2f, -0.2f,
      -0.05f, -0.3f,
      -0.05f, -0.2f,
      
      // Left Booster B
      -0.05f, -0.2f,
      -0.2f, -0.15f,
      -0.2f, -0.2f,

      // Right Booster A
      0.05f, -0.2f,
      0.05f, -0.3f,
      0.2f, -0.2f,

      // Right Booster B
      0.2f, -0.2f,
      0.2f, -0.15f,
      0.05f, -0.2f
  };

  glGenBuffers(1, &_vertexPositionsBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
}

BoosterMesh::~BoosterMesh() {
  glDeleteBuffers(1, &_vertexPositionsBuffer);
}

void BoosterMesh::bind() {
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void BoosterMesh::unbind() {
  glDisableVertexAttribArray(0);
}

void BoosterMesh::draw() {
  glDrawArrays(GL_TRIANGLES, 0, 12);
}

}  // namespace linguine::render
