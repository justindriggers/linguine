#include "TriangleMesh.h"

namespace linguine::render {

TriangleMesh::TriangleMesh() {
  GLfloat positions[] = {
      -0.5f, -0.5f,
       0.5f, -0.5f,
       0.0f,  0.5f
  };

  glGenBuffers(1, &_vertexPositionsBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
}

TriangleMesh::~TriangleMesh() {
  glDeleteBuffers(1, &_vertexPositionsBuffer);
}

void TriangleMesh::bind() {
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void TriangleMesh::unbind() {
  glDisableVertexAttribArray(0);
}

void TriangleMesh::draw() {
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

}  // namespace linguine::render
