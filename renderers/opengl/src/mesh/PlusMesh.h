#pragma once

#include <GLES3/gl3.h>

#include "Mesh.h"

namespace linguine::render {

class PlusMesh : public Mesh {
  public:
    explicit PlusMesh();

    ~PlusMesh() override;

    void bind() override;

    void unbind() override;

    void draw() override;

  private:
    GLuint _vertexPositionsBuffer{};
};

}  // namespace linguine::render
