#pragma once

#include "Mesh.h"

namespace linguine::render {

class ShipMesh : public Mesh {
  public:
    explicit ShipMesh(MTL::Device& device);

    ~ShipMesh() override;

    void bind(MTL::RenderCommandEncoder& encoder) override;

    void draw(MTL::RenderCommandEncoder& encoder) override;

  private:
    MTL::Buffer* _vertexPositionsBuffer;
};

}  // namespace linguine::render
