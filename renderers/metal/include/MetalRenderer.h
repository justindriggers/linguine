#pragma once

#include <Renderer.h>

namespace MTK {
  class View;
}

namespace linguine::render {
  class MetalRendererImpl;

  class MetalRenderer : public Renderer {
    public:
      virtual ~MetalRenderer() = default;

      static MetalRenderer* create(MTK::View& view);

    private:
      friend class MetalRendererImpl;

      MetalRenderer() = default;
  };
}