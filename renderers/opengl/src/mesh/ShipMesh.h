#pragma once

#include <GLES3/gl3.h>

#include "Mesh.h"

namespace linguine::render {

class ShipMesh : public Mesh {
  public:
    explicit ShipMesh();

    ~ShipMesh() override;

    void bind() override;

    void unbind() override;

    void draw() override;

  private:
    GLuint _vertexPositionsBuffer{};
};

}  // namespace linguine::render
