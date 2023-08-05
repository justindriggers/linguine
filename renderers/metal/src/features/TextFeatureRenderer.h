#pragma once

#include "renderer/features/FeatureRenderer.h"

#include <simd/simd.h>
#include <vector>

#include <Metal/Metal.hpp>

#include "MetalRenderContext.h"
#include "MetalTextureLoader.h"
#include "mesh/MeshRegistry.h"

namespace linguine::render {

class TextFeatureRenderer : public FeatureRenderer {
public:
  TextFeatureRenderer(MetalRenderContext& context,
                      MeshRegistry& meshRegistry,
                      MetalTextureLoader& textureLoader);

  ~TextFeatureRenderer() override;

  bool isRelevant(Renderable& renderable) override;

  void draw(Camera& camera) override;

  void resize(uint16_t width, uint16_t height) override {}

private:
  MetalRenderContext& _context;
  MeshRegistry& _meshRegistry;

  MTL::Texture* _fontTexture;

  std::vector<MTL::Buffer*> _cameraBuffers;
  MTL::RenderPipelineState* _pipelineState = nullptr;
  MTL::DepthStencilState* _depthState = nullptr;

  std::vector<std::vector<MTL::Buffer*>> _colorValueBuffers;
  std::vector<std::vector<std::vector<MTL::Buffer*>>> _glyphVertexValueBuffers;
  std::vector<std::vector<std::vector<MTL::Buffer*>>> _glyphFragmentValueBuffers;

  struct MetalCamera {
    simd::float4x4 viewProjectionMatrix{};
  };

  struct MetalColoredFeature {
    simd::float3 color{};
  };

  struct MetalGlyphVertexFeature {
    simd::float4x4 modelMatrix{};
  };

  struct MetalGlyphFragmentFeature {
    simd::float2 position{};
  };

  void ensureCameraBuffersCapacity(uint64_t maxId);

  const std::unordered_map<char, simd::float2> _glyphPositions = {
      { 'A', simd::float2{ 0.0f, 0.0f } },
      { 'B', simd::float2{ 10.0f, 0.0f } },
      { 'C', simd::float2{ 20.0f, 0.0f } },
      { 'D', simd::float2{ 30.0f, 0.0f } },
      { 'E', simd::float2{ 40.0f, 0.0f } },
      { 'F', simd::float2{ 50.0f, 0.0f } },
      { 'G', simd::float2{ 60.0f, 0.0f } },
      { 'H', simd::float2{ 70.0f, 0.0f } },
      { 'I', simd::float2{ 80.0f, 0.0f } },
      { 'J', simd::float2{ 90.0f, 0.0f } },
      { 'K', simd::float2{ 100.0f, 0.0f } },
      { 'L', simd::float2{ 110.0f, 0.0f } },
      { 'M', simd::float2{ 120.0f, 0.0f } },
      { 'N', simd::float2{ 130.0f, 0.0f } },
      { 'O', simd::float2{ 140.0f, 0.0f } },
      { 'P', simd::float2{ 150.0f, 0.0f } },
      { 'Q', simd::float2{ 0.0f, 10.0f } },
      { 'R', simd::float2{ 10.0f, 10.0f } },
      { 'S', simd::float2{ 20.0f, 10.0f } },
      { 'T', simd::float2{ 30.0f, 10.0f } },
      { 'U', simd::float2{ 40.0f, 10.0f } },
      { 'V', simd::float2{ 50.0f, 10.0f } },
      { 'W', simd::float2{ 60.0f, 10.0f } },
      { 'X', simd::float2{ 70.0f, 10.0f } },
      { 'Y', simd::float2{ 80.0f, 10.0f } },
      { 'Z', simd::float2{ 90.0f, 10.0f } },

      { 'a', simd::float2{ 100.0f, 10.0f } },
      { 'b', simd::float2{ 110.0f, 10.0f } },
      { 'c', simd::float2{ 120.0f, 10.0f } },
      { 'd', simd::float2{ 130.0f, 10.0f } },
      { 'e', simd::float2{ 140.0f, 10.0f } },
      { 'f', simd::float2{ 150.0f, 10.0f } },
      { 'g', simd::float2{ 0.0f, 20.0f } },
      { 'h', simd::float2{ 10.0f, 20.0f } },
      { 'i', simd::float2{ 20.0f, 20.0f } },
      { 'j', simd::float2{ 30.0f, 20.0f } },
      { 'k', simd::float2{ 40.0f, 20.0f } },
      { 'l', simd::float2{ 50.0f, 20.0f } },
      { 'm', simd::float2{ 60.0f, 20.0f } },
      { 'n', simd::float2{ 70.0f, 20.0f } },
      { 'o', simd::float2{ 80.0f, 20.0f } },
      { 'p', simd::float2{ 90.0f, 20.0f } },
      { 'q', simd::float2{ 100.0f, 20.0f } },
      { 'r', simd::float2{ 110.0f, 20.0f } },
      { 's', simd::float2{ 120.0f, 20.0f } },
      { 't', simd::float2{ 130.0f, 20.0f } },
      { 'u', simd::float2{ 140.0f, 20.0f } },
      { 'v', simd::float2{ 150.0f, 20.0f } },
      { 'w', simd::float2{ 0.0f, 30.0f } },
      { 'x', simd::float2{ 10.0f, 30.0f } },
      { 'y', simd::float2{ 20.0f, 30.0f } },
      { 'z', simd::float2{ 30.0f, 30.0f } },

      { '1', simd::float2{ 40.0f, 30.0f } },
      { '2', simd::float2{ 50.0f, 30.0f } },
      { '3', simd::float2{ 60.0f, 30.0f } },
      { '4', simd::float2{ 70.0f, 30.0f } },
      { '5', simd::float2{ 80.0f, 30.0f } },
      { '6', simd::float2{ 90.0f, 30.0f } },
      { '7', simd::float2{ 100.0f, 30.0f } },
      { '8', simd::float2{ 110.0f, 30.0f } },
      { '9', simd::float2{ 120.0f, 30.0f } },
      { '0', simd::float2{ 130.0f, 30.0f } },

      { '.', simd::float2{ 140.0f, 30.0f } },
      { ',', simd::float2{ 150.0f, 30.0f } },
      { '!', simd::float2{ 0.0f, 40.0f } },
      { '?', simd::float2{ 10.0f, 40.0f } },
      { '(', simd::float2{ 20.0f, 40.0f } },
      { ')', simd::float2{ 30.0f, 40.0f } },
      { '+', simd::float2{ 40.0f, 40.0f } },
      { '-', simd::float2{ 50.0f, 40.0f } },
      { '=', simd::float2{ 60.0f, 40.0f } },
      { '/', simd::float2{ 70.0f, 40.0f } },
      { '\'', simd::float2{ 80.0f, 40.0f } },
      { ' ', simd::float2{ 90.0f, 40.0f } }
  };
};

}  // namespace linguine::render
