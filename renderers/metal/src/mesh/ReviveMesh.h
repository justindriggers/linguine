#pragma once

#include "Mesh.h"

namespace linguine::render {

class ReviveMesh : public Mesh {
  public:
    explicit ReviveMesh(MTL::Device& device);

    ~ReviveMesh() override;

    void bind(MTL::RenderCommandEncoder& encoder) override;

    void draw(MTL::RenderCommandEncoder& encoder) override;

  private:
    MTL::Buffer* _vertexPositionsBuffer;
};

}  // namespace linguine::render
