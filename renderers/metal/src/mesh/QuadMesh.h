#pragma once

#include "Mesh.h"

namespace linguine::render {

class QuadMesh : public Mesh {
  public:
    explicit QuadMesh(MTL::Device& device);

    ~QuadMesh() override;

    void bind(MTL::RenderCommandEncoder& encoder) override;

    void draw(MTL::RenderCommandEncoder& encoder) override;

  private:
    MTL::Buffer* _vertexPositionsBuffer;
};

}  // namespace linguine::render
