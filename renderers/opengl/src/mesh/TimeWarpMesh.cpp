#include "TimeWarpMesh.h"

namespace linguine::render {

TimeWarpMesh::TimeWarpMesh() {
  GLfloat positions[] = {
      // Base
      -0.375f, -0.475f,
       0.375f, -0.325f,
      -0.375f, -0.325f,

       0.375f, -0.325f,
      -0.375f, -0.475f,
       0.375f, -0.475f,

      // Cap
      -0.375f,  0.325f,
       0.375f,  0.475f,
      -0.375f,  0.475f,

       0.375f,  0.475f,
      -0.375f,  0.325f,
       0.375f,  0.325f,

      // Bottom
      -0.375f, -0.325f,
       0.0f,   -0.325f,
      -0.125f,  0.0f,

      -0.125f,  0.0f,
       0.0f,   -0.325f,
       0.125f,  0.0f,

       0.375f, -0.325f,
       0.125f,  0.0f,
       0.0f,   -0.325f,

      // Top
      -0.375f,  0.325f,
      -0.125f,  0.0f,
       0.0f,    0.325f,

      -0.125f,  0.0f,
       0.125f,  0.0f,
       0.0f,    0.325f,

       0.125f,  0.0f,
       0.375f,  0.325f,
       0.0f,    0.325f
  };

  glGenBuffers(1, &_vertexPositionsBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
}

TimeWarpMesh::~TimeWarpMesh() {
  glDeleteBuffers(1, &_vertexPositionsBuffer);
}

void TimeWarpMesh::bind() {
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void TimeWarpMesh::unbind() {
  glDisableVertexAttribArray(0);
}

void TimeWarpMesh::draw() {
  glDrawArrays(GL_TRIANGLES, 0, 30);
}

}  // namespace linguine::render
