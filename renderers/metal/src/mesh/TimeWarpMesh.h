#pragma once

#include "Mesh.h"

namespace linguine::render {

class TimeWarpMesh : public Mesh {
  public:
    explicit TimeWarpMesh(MTL::Device& device);

    ~TimeWarpMesh() override;

    void bind(MTL::RenderCommandEncoder& encoder) override;

    void draw(MTL::RenderCommandEncoder& encoder) override;

  private:
    MTL::Buffer* _vertexPositionsBuffer;
};

}  // namespace linguine::render
