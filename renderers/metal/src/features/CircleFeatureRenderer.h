#pragma once

#include "renderer/features/FeatureRenderer.h"

#include <simd/simd.h>
#include <vector>

#include <Metal/Metal.hpp>

#include "mesh/MeshRegistry.h"
#include "MetalRenderContext.h"

namespace linguine::render {

class CircleFeatureRenderer : public FeatureRenderer {
  public:
    CircleFeatureRenderer(MetalRenderContext& context,
                          MeshRegistry& meshRegistry);

    ~CircleFeatureRenderer() override;

    bool isRelevant(Renderable& renderable) override;

    void draw(Camera& camera) override;

    void resize(uint16_t width, uint16_t height) override {}

  private:
    MetalRenderContext& _context;
    MeshRegistry& _meshRegistry;

    std::vector<MTL::Buffer*> _cameraBuffers;
    MTL::RenderPipelineState* _pipelineState = nullptr;
    MTL::DepthStencilState* _depthState = nullptr;

    std::vector<std::vector<MTL::Buffer*>> _valueBuffers;

    struct MetalCamera {
      simd::float4x4 viewProjectionMatrix{};
    };

    struct MetalCircleFeature {
      simd::float4x4 modelMatrix{};
      simd::float3 color{};
    };

    void ensureCameraBuffersCapacity(uint64_t maxId);
};

}  // namespace linguine::render
