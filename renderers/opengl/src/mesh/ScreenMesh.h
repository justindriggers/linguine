#pragma once

#include "Mesh.h"

#include <GLES3/gl3.h>

namespace linguine::render {

class ScreenMesh : public Mesh {
  public:
    ScreenMesh();

    ~ScreenMesh() override;

    void bind() override;

    void unbind() override;

    void draw() override;

  private:
    GLuint _vertexPositionsBuffer{};
};

}  // namespace linguine::render
