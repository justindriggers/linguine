#pragma once

#include "renderer/features/FeatureRenderer.h"

#include <Metal/Metal.hpp>

#include "../MetalRenderContext.h"

namespace linguine::render {

class QuadFeatureRenderer : public FeatureRenderer {
  public:
    explicit QuadFeatureRenderer(MetalRenderContext& context);

    ~QuadFeatureRenderer() override;

    bool isRelevant(Renderable& renderable) override;

    void draw() override;

  private:
    MetalRenderContext& _context;

    MTL::Buffer* _vertexPositionsBuffer = nullptr;
    MTL::RenderPipelineState* _pipelineState = nullptr;

    std::vector<MTL::Buffer*> _valueBuffers;
};

}  // namespace linguine::render
