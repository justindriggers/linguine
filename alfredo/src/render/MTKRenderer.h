#pragma once

#include <Renderer.h>

namespace MTK {
  class View;
}

namespace linguine::alfredo {
  class MTKRenderer : public Renderer {
    public:
      explicit MTKRenderer(MTK::View& view) : _view(view) {}

      void draw() override;

    private:
      MTK::View& _view;
  };
}