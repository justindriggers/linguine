#pragma once

#include "Mesh.h"

namespace linguine::render {

class WingMesh : public Mesh {
  public:
    explicit WingMesh(MTL::Device& device);

    ~WingMesh() override;

    void bind(MTL::RenderCommandEncoder& encoder) override;

    void draw(MTL::RenderCommandEncoder& encoder) override;

  private:
    MTL::Buffer* _vertexPositionsBuffer;
};

}  // namespace linguine::render
