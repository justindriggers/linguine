#pragma once

#include "renderer/features/FeatureRenderer.h"

#include <simd/simd.h>
#include <vector>

#include <Metal/Metal.hpp>

#include <renderer/Camera.h>

#include "mesh/MeshRegistry.h"
#include "MetalRenderContext.h"

namespace linguine::render {

class ProgressFeatureRenderer : public FeatureRenderer {
  public:
    ProgressFeatureRenderer(MetalRenderContext& context, Camera& camera,
                            MeshRegistry& meshRegistry);

    ~ProgressFeatureRenderer() override;

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

    std::vector<MTL::Buffer*> _vertexFeatureBuffers;
    std::vector<MTL::Buffer*> _fragmentFeatureBuffers;

    struct MetalCamera {
      simd::float4x4 viewProjectionMatrix{};
    };

    struct MetalProgressVertexFeature {
      simd::float4x4 modelMatrix{};
    };

    struct MetalProgressFragmentFeature {
      simd::float3 color{};
      float progress{};
    };
};

}  // namespace linguine::render
