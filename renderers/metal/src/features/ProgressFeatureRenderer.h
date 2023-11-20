#pragma once

#include "renderer/features/FeatureRenderer.h"

#include <simd/simd.h>
#include <unordered_map>
#include <vector>

#include <Metal/Metal.hpp>

#include "mesh/MeshRegistry.h"
#include "MetalRenderContext.h"

namespace linguine::render {

class ProgressFeatureRenderer : public FeatureRenderer {
  public:
    ProgressFeatureRenderer(MetalRenderContext& context,
                            MeshRegistry& meshRegistry);

    ~ProgressFeatureRenderer() override;

    bool isRelevant(Renderable& renderable) override;

    void onFrameBegin() override {}

    void draw(Camera& camera) override;

    void resize(uint16_t width, uint16_t height) override {}

    void reset() override;

  private:
    MetalRenderContext& _context;
    MeshRegistry& _meshRegistry;

    std::unordered_map<uint64_t, MTL::Buffer*> _cameraBuffers;
    MTL::RenderPipelineState* _pipelineState = nullptr;
    MTL::DepthStencilState* _depthState = nullptr;

    std::unordered_map<uint64_t, std::vector<MTL::Buffer*>> _vertexFeatureBuffers;
    std::unordered_map<uint64_t, std::vector<MTL::Buffer*>> _fragmentFeatureBuffers;

    struct MetalCamera {
      simd::float4x4 viewProjectionMatrix{};
    };

    struct MetalProgressVertexFeature {
      simd::float4x4 modelMatrix{};
    };

    struct MetalProgressFragmentFeature {
      simd::float4 backgroundColor{};
      simd::float3 color{};
      float progress{};
    };

    void ensureCameraBuffers(uint64_t cameraId);
};

}  // namespace linguine::render
