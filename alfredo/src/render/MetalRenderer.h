#pragma once

#include <Renderer.h>

#include <memory>

namespace MTK {
  class View;
}

namespace linguine::alfredo {
  class MetalRendererImpl;

  class MetalRenderer : public Renderer {
    public:
      virtual ~MetalRenderer() = default;

      static MetalRenderer* create(MTK::View& view);

      virtual void drawInternal() = 0;

    private:
      friend class MetalRendererImpl;

      MetalRenderer() = default;
  };
}