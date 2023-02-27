#pragma once

#include "Mesh.h"

namespace linguine::render {

class TriangleMesh : public Mesh {
  public:
    explicit TriangleMesh(MTL::Device& device);

    ~TriangleMesh() override;

    void bind(MTL::RenderCommandEncoder& encoder) override;

    void draw(MTL::RenderCommandEncoder& encoder) override;

  private:
    MTL::Buffer* _vertexPositionsBuffer;
};

}  // namespace linguine::render
