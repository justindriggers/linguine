#pragma once

#include "Mesh.h"

namespace linguine::render {

class PlusMesh : public Mesh {
  public:
    explicit PlusMesh(MTL::Device& device);

    ~PlusMesh() override;

    void bind(MTL::RenderCommandEncoder& encoder) override;

    void draw(MTL::RenderCommandEncoder& encoder) override;

  private:
    MTL::Buffer* _vertexPositionsBuffer;
};

}  // namespace linguine::render
