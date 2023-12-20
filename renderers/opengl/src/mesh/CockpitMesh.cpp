#include "CockpitMesh.h"

namespace linguine::render {

CockpitMesh::CockpitMesh() {
  GLfloat positions[] = {
      0.0f, 0.2f,
      -0.1f, -0.1f,
      0.1f, -0.1f,
      
      -0.1f, -0.1f,
      0.0f, -0.15f,
      0.1f, -0.1f
  };

  glGenBuffers(1, &_vertexPositionsBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
}

CockpitMesh::~CockpitMesh() {
  glDeleteBuffers(1, &_vertexPositionsBuffer);
}

void CockpitMesh::bind() {
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexPositionsBuffer);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void CockpitMesh::unbind() {
  glDisableVertexAttribArray(0);
}

void CockpitMesh::draw() {
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

}  // namespace linguine::render
