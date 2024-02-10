#include "SpeedBoostMesh.h"

namespace linguine::render {

SpeedBoostMesh::SpeedBoostMesh() {
  GLfloat positions[] = {
      // Bottom Bottom-Left
      -0.425f, -0.5f,
       0.0f,   -0.25f,
      -0.425f, -0.25f,

      // Bottom Bottom-Right
       0.425f, -0.5f,
       0.425f, -0.25f,
       0.0f,   -0.25f,

      // Bottom Center
      -0.425f, -0.25f,
       0.425f, -0.25f,
       0.0f,    0.0f,

      // Top Bottom-Left
      -0.425f,  0.0f,
       0.0f,    0.25f,
      -0.425f,  0.25f,

      // Top Bottom-Right
       0.425f,  0.0f,
       0.425f,  0.25f,
       0.0f,    0.25f,

      // Top Center
      -0.425f,  0.25f,
       0.425f,  0.25f,
       0.0f,    0.5f
  };

  glGenBuffers(1, &_vertexPositionsBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
}

SpeedBoostMesh::~SpeedBoostMesh() {
  glDeleteBuffers(1, &_vertexPositionsBuffer);
}

void SpeedBoostMesh::bind() {
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void SpeedBoostMesh::unbind() {
  glDisableVertexAttribArray(0);
}

void SpeedBoostMesh::draw() {
  glDrawArrays(GL_TRIANGLES, 0, 18);
}

}  // namespace linguine::render
