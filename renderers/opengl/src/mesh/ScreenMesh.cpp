#include "ScreenMesh.h"

namespace linguine::render {

ScreenMesh::ScreenMesh() {
  GLfloat positions[] = {
      -1.0f, -1.0f,
       1.0f, -1.0f,
      -1.0f,  1.0f,
      
      -1.0f,  1.0f,
       1.0f, -1.0f,
       1.0f, 1.0f
  };

  glGenBuffers(1, &_vertexPositionsBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
}

ScreenMesh::~ScreenMesh() {
  glDeleteBuffers(1, &_vertexPositionsBuffer);
}

void ScreenMesh::bind() {
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void ScreenMesh::unbind() {
  glDisableVertexAttribArray(0);
}

void ScreenMesh::draw() {
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

}  // namespace linguine::render
