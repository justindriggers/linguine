#include "QuadMesh.h"

namespace linguine::render {

QuadMesh::QuadMesh() {
  GLfloat positions[] = {
      -0.5f, -0.5f,
       0.5f, -0.5f,
      -0.5f,  0.5f,

      -0.5f,  0.5f,
       0.5f, -0.5f,
       0.5f, 0.5f
  };

  glGenBuffers(1, &_vertexPositionsBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
}

QuadMesh::~QuadMesh() {
  glDeleteBuffers(1, &_vertexPositionsBuffer);
}

void QuadMesh::bind() {
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void QuadMesh::unbind() {
  glDisableVertexAttribArray(0);
}

void QuadMesh::draw() {
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

}  // namespace linguine::render
