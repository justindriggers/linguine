#include "WingMesh.h"

namespace linguine::render {

WingMesh::WingMesh() {
  GLfloat positions[] = {
      // Left Wing
      -0.4f, -0.1f,
      -0.2f, -0.15f,
      -0.2f, 0.2f,
      
      // Body A
      -0.2f, -0.15f,
      0.2f, 0.2f,
      -0.2f, 0.2f,

      // Body B
      -0.2f, -0.15f,
      0.2f, -0.15f,
      0.2f, 0.2f,

      // Right Wing
      0.4f, -0.1f,
      0.2f, 0.2f,
      0.2f, -0.15f
  };

  glGenBuffers(1, &_vertexPositionsBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
}

WingMesh::~WingMesh() {
  glDeleteBuffers(1, &_vertexPositionsBuffer);
}

void WingMesh::bind() {
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void WingMesh::unbind() {
  glDisableVertexAttribArray(0);
}

void WingMesh::draw() {
  glDrawArrays(GL_TRIANGLES, 0, 12);
}

}  // namespace linguine::render
