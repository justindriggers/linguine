#include "ReviveMesh.h"

namespace linguine::render {

ReviveMesh::ReviveMesh() {
  GLfloat positions[] = {
       0.125f,  0.075f,
      -0.125f,  0.5f,
      -0.25f,   0.075f,

      -0.125f,  0.5f,
       0.125f,  0.075f,
       0.25f,   0.5f,

      -0.125f,  0.075f,
      -0.25f,  -0.5f,
       0.25f,   0.075f
  };

  glGenBuffers(1, &_vertexPositionsBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
}

ReviveMesh::~ReviveMesh() {
  glDeleteBuffers(1, &_vertexPositionsBuffer);
}

void ReviveMesh::bind() {
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void ReviveMesh::unbind() {
  glDisableVertexAttribArray(0);
}

void ReviveMesh::draw() {
  glDrawArrays(GL_TRIANGLES, 0, 9);
}

}  // namespace linguine::render
