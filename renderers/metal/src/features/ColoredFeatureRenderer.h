#pragma once

#include "renderer/features/FeatureRenderer.h"

#include <simd/simd.h>
#include <vector>

#include <Metal/Metal.hpp>

#include <renderer/Camera.h>

#include "mesh/MeshRegistry.h"
#include "MetalRenderContext.h"

namespace linguine::render {

class ColoredFeatureRenderer : public FeatureRenderer {
  public:
    ColoredFeatureRenderer(MetalRenderContext& context, Camera& camera,
                           MeshRegistry& meshRegistry);

    ~ColoredFeatureRenderer() override;

    bool isRelevant(Renderable& renderable) override;

    void draw() override;

    void resize(uint16_t width, uint16_t height) override {}

  private:
    MetalRenderContext& _context;
    Camera& _camera;
    MeshRegistry& _meshRegistry;

    MTL::Buffer* _cameraBuffer = nullptr;
    MTL::RenderPipelineState* _pipelineState = nullptr;
    MTL::DepthStencilState* _depthState = nullptr;

    std::vector<MTL::Buffer*> _valueBuffers;

    struct MetalCamera {
      simd::float4x4 viewProjectionMatrix{};
    };

    struct MetalColoredFeature {
      simd::float4x4 modelMatrix{};
      simd::float3 color{};
    };
};

}  // namespace linguine::render
