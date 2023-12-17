#include "PlusMesh.h"

namespace linguine::render {

PlusMesh::PlusMesh() {
  GLfloat positions[] = {
      -0.25f, -0.25f,
      0.25f, 0.25f,
      -0.25f, 0.25f,
      
      -0.25f, -0.25f,
      0.25f, -0.25f,
      0.25f, 0.25f,

      -0.5f, -0.25f,
      -0.25f, 0.25f,
      -0.5f,  0.25f,

      -0.5f, -0.25f,
      -0.25f, -0.25f,
      -0.25f, 0.25f,

      -0.25f, 0.25f,
      0.25f, 0.5f,
      -0.25f, 0.5f,

      -0.25f, 0.25f,
      0.25f, 0.25f,
      0.25f, 0.5f,

      0.25f, -0.25f,
      0.5f, 0.25f,
      0.25f, 0.25f,

      0.25f, -0.25f,
      0.5f, -0.25f,
      0.5f, 0.25f,

      -0.25f, -0.5f,
      0.25f, -0.25f,
      -0.25f, -0.25f,

      -0.25f, -0.5f,
      0.25f, -0.5f,
      0.25f, -0.25f
  };

  glGenBuffers(1, &_vertexPositionsBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
}

PlusMesh::~PlusMesh() {
  glDeleteBuffers(1, &_vertexPositionsBuffer);
}

void PlusMesh::bind() {
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void PlusMesh::unbind() {
  glDisableVertexAttribArray(0);
}

void PlusMesh::draw() {
  glDrawArrays(GL_TRIANGLES, 0, 30);
}

}  // namespace linguine::render
