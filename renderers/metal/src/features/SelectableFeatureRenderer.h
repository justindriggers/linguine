#pragma once

#include "renderer/features/FeatureRenderer.h"

#include <simd/simd.h>
#include <unordered_map>
#include <vector>

#include <Metal/Metal.hpp>

#include "mesh/MeshRegistry.h"
#include "MetalRenderContext.h"

namespace linguine::render {

class SelectableFeatureRenderer : public FeatureRenderer {
  public:
    SelectableFeatureRenderer(MetalRenderContext& context,
                              MeshRegistry& meshRegistry);

    ~SelectableFeatureRenderer() override;

    bool isRelevant(Renderable& renderable) override;

    void onFrameBegin() override;

    void draw(Camera& camera) override;

    void resize(uint16_t width, uint16_t height) override;

    void reset() override;

    [[nodiscard]] std::optional<uint64_t> getEntityIdAt(float x, float y) const;

    static simd::uint2 toUint2(uint64_t value) {
      simd::uint2 result;

      result.x = static_cast<simd::uint1>(value);
      result.y = static_cast<simd::uint1>(value >> 32);

      return result;
    }

    static uint64_t toUint64(simd::uint2 value) {
      uint64_t result = value.y;
      result = result << 32;
      result = result | value.x;

      return result;
    }

  private:
    MetalRenderContext& _context;
    MeshRegistry& _meshRegistry;

    std::unordered_map<uint64_t, MTL::Buffer*> _cameraBuffers;
    MTL::RenderPipelineState* _pipelineState = nullptr;
    MTL::DepthStencilState* _depthState = nullptr;
    MTL::Texture* _selectableTexture = nullptr;
    MTL::Texture* _selectableDepthTexture = nullptr;
    MTL::RenderPassDescriptor* _selectableRenderPassDescriptor = nullptr;

    std::unordered_map<uint64_t, std::vector<MTL::Buffer*>> _valueBuffers;

    struct MetalCamera {
      simd::float4x4 viewProjectionMatrix{};
    };

    struct MetalSelectableFeature {
      simd::float4x4 modelMatrix{};
      simd::uint2 entityId{};
    };

    void ensureCameraBuffers(uint64_t cameraId);
};

}  // namespace linguine::render
