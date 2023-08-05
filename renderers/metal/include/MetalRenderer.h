#pragma once

#include "renderer/Renderer.h"

#include "MetalTextureLoader.h"

namespace MTK {

  class View;

}  // namespace MTK

namespace linguine::render {

class MetalRendererImpl;

class MetalRenderer : public Renderer {
  public:
    ~MetalRenderer() override = default;

    virtual void doDraw() = 0;

    static MetalRenderer* create(MTK::View& view, bool autoDraw,
                                 MetalTextureLoader& textureLoader);

  private:
    friend class MetalRendererImpl;

    MetalRenderer() = default;
};

}  // namespace linguine::render
