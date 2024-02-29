#pragma once

#include "renderer/RenderBackend.h"

#include "MetalTextureLoader.h"

namespace MTK {

  class View;

}  // namespace MTK

namespace linguine::render {

class MetalRenderBackendImpl;

class MetalRenderBackend : public RenderBackend {
  public:
    ~MetalRenderBackend() override = default;

    static std::unique_ptr<MetalRenderBackend> create(MTK::View& view, std::unique_ptr<MetalTextureLoader> textureLoader);

  private:
    friend class MetalRenderBackendImpl;

    MetalRenderBackend() = default;
};

}  // namespace linguine::render
