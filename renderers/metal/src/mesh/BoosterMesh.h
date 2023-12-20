#pragma once

#include "Mesh.h"

namespace linguine::render {

class BoosterMesh : public Mesh {
  public:
    explicit BoosterMesh(MTL::Device& device);

    ~BoosterMesh() override;

    void bind(MTL::RenderCommandEncoder& encoder) override;

    void draw(MTL::RenderCommandEncoder& encoder) override;

  private:
    MTL::Buffer* _vertexPositionsBuffer;
};

}  // namespace linguine::render
