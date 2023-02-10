#pragma once

#include "renderer/features/FeatureRenderer.h"

#include <Metal/Metal.hpp>

#include "renderer/features/QuadFeature.h"

#include "../MetalRenderContext.h"

namespace linguine::render {

class TriangleFeatureRenderer : public FeatureRenderer {
  public:
    explicit TriangleFeatureRenderer(MetalRenderContext& context);

    ~TriangleFeatureRenderer() override;

    bool isRelevant(Renderable& renderable) override;

    void draw() override;

  private:
    MetalRenderContext& _context;

    MTL::Buffer* _vertexPositionsBuffer = nullptr;
    MTL::RenderPipelineState* _pipelineState = nullptr;
};

}  // namespace linguine::render