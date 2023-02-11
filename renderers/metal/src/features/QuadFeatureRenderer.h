#pragma once

#include "renderer/features/FeatureRenderer.h"

#include <simd/simd.h>

#include <Metal/Metal.hpp>

#include <renderer/Camera.h>

#include "../MetalRenderContext.h"

namespace linguine::render {

class QuadFeatureRenderer : public FeatureRenderer {
  public:
    explicit QuadFeatureRenderer(MetalRenderContext& context, Camera& camera);

    ~QuadFeatureRenderer() override;

    bool isRelevant(Renderable& renderable) override;

    void draw() override;

  private:
    MetalRenderContext& _context;
    Camera& _camera;

    MTL::Buffer* _cameraBuffer = nullptr;
    MTL::Buffer* _vertexPositionsBuffer = nullptr;
    MTL::RenderPipelineState* _pipelineState = nullptr;
    MTL::DepthStencilState* _depthState = nullptr;

    std::vector<MTL::Buffer*> _valueBuffers;

    struct MetalCamera {
      simd::float4x4 viewProjectionMatrix{};
    };

    struct MetalQuadFeature {
      simd::float4x4 modelMatrix{};
      simd::float3 color{};
    };
};

}  // namespace linguine::render
