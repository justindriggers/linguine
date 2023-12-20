#pragma once

#include "Mesh.h"

namespace linguine::render {

class CockpitMesh : public Mesh {
  public:
    explicit CockpitMesh(MTL::Device& device);

    ~CockpitMesh() override;

    void bind(MTL::RenderCommandEncoder& encoder) override;

    void draw(MTL::RenderCommandEncoder& encoder) override;

  private:
    MTL::Buffer* _vertexPositionsBuffer;
};

}  // namespace linguine::render
