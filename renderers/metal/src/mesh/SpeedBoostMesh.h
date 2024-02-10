#pragma once

#include "Mesh.h"

namespace linguine::render {

class SpeedBoostMesh : public Mesh {
  public:
    explicit SpeedBoostMesh(MTL::Device& device);

    ~SpeedBoostMesh() override;

    void bind(MTL::RenderCommandEncoder& encoder) override;

    void draw(MTL::RenderCommandEncoder& encoder) override;

  private:
    MTL::Buffer* _vertexPositionsBuffer;
};

}  // namespace linguine::render
