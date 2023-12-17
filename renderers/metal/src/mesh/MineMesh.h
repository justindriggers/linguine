#pragma once

#include "Mesh.h"

namespace linguine::render {

class MineMesh : public Mesh {
  public:
    explicit MineMesh(MTL::Device& device);

    ~MineMesh() override;

    void bind(MTL::RenderCommandEncoder& encoder) override;

    void draw(MTL::RenderCommandEncoder& encoder) override;

  private:
    MTL::Buffer* _vertexPositionsBuffer;
};

}  // namespace linguine::render
