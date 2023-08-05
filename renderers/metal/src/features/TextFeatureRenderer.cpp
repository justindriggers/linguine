#include "TextFeatureRenderer.h"

#include "renderer/features/TextFeature.h"

#include <glm/ext/matrix_transform.hpp>

namespace linguine::render {

TextFeatureRenderer::TextFeatureRenderer(MetalRenderContext& context,
                                         MeshRegistry& meshRegistry,
                                         MetalTextureLoader& textureLoader)
    : _context(context), _meshRegistry(meshRegistry) {
  _fontTexture = textureLoader.getTexture();

  const auto shaderSourceCode = R"(
      struct MetalCamera {
        metal::float4x4 viewProjectionMatrix;
      };

      struct MetalColoredFeature {
        float3 color;
      };

      struct MetalGlyphVertexFeature {
        metal::float4x4 modelMatrix;
      };

      struct VertexOutput {
        float4 position [[position]];
        float3 color;
        float2 uv;
      };

      struct MetalGlyphFragmentFeature {
        float2 position;
      };

      VertexOutput vertex vertexText(uint index [[vertex_id]],
          const device float2* positions [[buffer(0)]],
          const constant MetalCamera& camera [[buffer(1)]],
          const constant MetalColoredFeature& color [[buffer(2)]],
          const constant MetalGlyphVertexFeature& glyph [[buffer(3)]]) {
        VertexOutput o;
        o.position = camera.viewProjectionMatrix * glyph.modelMatrix * float4(positions[index], 0.0, 1.0);
        o.color = color.color;
        o.uv = positions[index];
        return o;
      }

      float4 fragment fragmentText(VertexOutput in [[stage_in]],
          metal::texture2d<float, metal::access::sample> fontTexture [[texture(0)]],
          const constant MetalGlyphFragmentFeature& feature [[buffer(0)]]) {
        constexpr auto nearestPixel = metal::sampler(
            metal::coord::pixel,
            metal::address::clamp_to_edge,
            metal::filter::nearest
        );

        float x = feature.position.x + metal::floor(10.0 * (in.uv.x + 0.5));
        float y = feature.position.y + metal::floor(10.0 * (-in.uv.y + 0.5));
        float sample = fontTexture.sample(nearestPixel, float2(x, y)).r;
        return float4(in.color, sample);
      }
    )";

  NS::Error* error = nullptr;
  const auto library = context.device->newLibrary(NS::String::string(shaderSourceCode, NS::StringEncoding::UTF8StringEncoding), nullptr, &error);

  if (!library) {
    throw std::runtime_error(error->localizedDescription()->utf8String());
  }

  const auto vertexFunction = library->newFunction(NS::String::string("vertexText", NS::StringEncoding::UTF8StringEncoding));
  const auto fragmentFunction = library->newFunction(NS::String::string("fragmentText", NS::StringEncoding::UTF8StringEncoding));

  const auto renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
  renderPipelineDescriptor->setVertexFunction(vertexFunction);
  renderPipelineDescriptor->setFragmentFunction(fragmentFunction);

  auto colorAttachment = renderPipelineDescriptor->colorAttachments()->object(0);
  colorAttachment->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
  colorAttachment->setBlendingEnabled(true);
  colorAttachment->setRgbBlendOperation(MTL::BlendOperation::BlendOperationAdd);
  colorAttachment->setAlphaBlendOperation(MTL::BlendOperation::BlendOperationAdd);
  colorAttachment->setSourceRGBBlendFactor(MTL::BlendFactor::BlendFactorSourceAlpha);
  colorAttachment->setSourceAlphaBlendFactor(MTL::BlendFactor::BlendFactorSourceAlpha);
  colorAttachment->setDestinationRGBBlendFactor(MTL::BlendFactor::BlendFactorOneMinusSourceAlpha);
  colorAttachment->setDestinationAlphaBlendFactor(MTL::BlendFactor::BlendFactorOneMinusSourceAlpha);

  renderPipelineDescriptor->setDepthAttachmentPixelFormat(MTL::PixelFormat::PixelFormatDepth32Float);

  _pipelineState = context.device->newRenderPipelineState(renderPipelineDescriptor, &error);

  if (!_pipelineState) {
    throw std::runtime_error(error->localizedDescription()->utf8String());
  }

  const auto depthStencilDescriptor = MTL::DepthStencilDescriptor::alloc()->init();
  depthStencilDescriptor->setDepthCompareFunction(MTL::CompareFunction::CompareFunctionLessEqual);
  depthStencilDescriptor->setDepthWriteEnabled(true);

  _depthState = context.device->newDepthStencilState(depthStencilDescriptor);

  if (!_depthState) {
    throw std::runtime_error(error->localizedDescription()->utf8String());
  }

  depthStencilDescriptor->release();
  vertexFunction->release();
  fragmentFunction->release();
  renderPipelineDescriptor->release();
  library->release();
}

TextFeatureRenderer::~TextFeatureRenderer() {
  for (const auto& cameraGlyphValueBuffers : _glyphVertexValueBuffers) {
    for (const auto& glyphValueBuffers : cameraGlyphValueBuffers) {
      for (const auto& glyphValueBuffer : glyphValueBuffers) {
        glyphValueBuffer->release();
      }
    }
  }

  for (const auto& cameraGlyphValueBuffers : _glyphFragmentValueBuffers) {
    for (const auto& glyphValueBuffers : cameraGlyphValueBuffers) {
      for (const auto& glyphValueBuffer : glyphValueBuffers) {
        glyphValueBuffer->release();
      }
    }
  }

  for (const auto& cameraColorValueBuffers : _colorValueBuffers) {
    for (const auto& colorValueBuffer : cameraColorValueBuffers) {
      colorValueBuffer->release();
    }
  }

  for (const auto& cameraBuffer : _cameraBuffers) {
    cameraBuffer->release();
  }

  _pipelineState->release();
  _depthState->release();
}

bool TextFeatureRenderer::isRelevant(Renderable& renderable) {
  return renderable.hasFeature<TextFeature>();
}

void TextFeatureRenderer::draw(Camera& camera) {
  auto commandEncoder = _context.commandBuffer->renderCommandEncoder(_context.coloredRenderPassDescriptor);

  commandEncoder->setRenderPipelineState(_pipelineState);
  commandEncoder->setDepthStencilState(_depthState);

  ensureCameraBuffersCapacity(camera.getId());

  auto cameraBuffer = _cameraBuffers[camera.getId()];
  auto metalCamera = static_cast<MetalCamera*>(cameraBuffer->contents());
  memcpy(&metalCamera->viewProjectionMatrix, &camera.viewProjectionMatrix, sizeof(simd::float4x4));
  commandEncoder->setVertexBuffer(cameraBuffer, 0, 1);

  auto& mesh = _meshRegistry.get(MeshType::Quad);
  mesh->bind(*commandEncoder);

  commandEncoder->setFragmentTexture(_fontTexture, 0);

  auto filteredRenderables = std::vector<Renderable*>();

  for (const auto& renderable : getRenderables()) {
    if (renderable.second->getLayer() == camera.layer && renderable.second->isEnabled()) {
      filteredRenderables.push_back(renderable.second);
    }
  }

  auto& colorValueBuffers = _colorValueBuffers[camera.getId()];

  while (colorValueBuffers.size() < filteredRenderables.size()) {
    colorValueBuffers.push_back(_context.device->newBuffer(sizeof(MetalColoredFeature), MTL::ResourceStorageModeShared));
    _glyphVertexValueBuffers[camera.getId()].emplace_back();
    _glyphFragmentValueBuffers[camera.getId()].emplace_back();
  }

  auto colorValueBufferIndex = 0;

  for (const auto& renderable : filteredRenderables) {
    auto feature = renderable->getFeature<TextFeature>();

    auto colorValueBuffer = colorValueBuffers[colorValueBufferIndex];
    auto metalColoredFeature = static_cast<MetalColoredFeature*>(colorValueBuffer->contents());
    memcpy(&metalColoredFeature->color, &feature.color, sizeof(simd::float3));

    commandEncoder->setVertexBuffer(colorValueBuffer, 0, 2);

    auto& glyphVertexValueBuffers = _glyphVertexValueBuffers[camera.getId()][colorValueBufferIndex];

    while (glyphVertexValueBuffers.size() < feature.text.size()) {
      glyphVertexValueBuffers.push_back(_context.device->newBuffer(sizeof(MetalGlyphVertexFeature), MTL::ResourceStorageModeShared));
    }

    auto& glyphFragmentValueBuffers = _glyphFragmentValueBuffers[camera.getId()][colorValueBufferIndex];

    while (glyphFragmentValueBuffers.size() < feature.text.size()) {
      glyphFragmentValueBuffers.push_back(_context.device->newBuffer(sizeof(MetalGlyphFragmentFeature), MTL::ResourceStorageModeShared));
    }

    auto glyphValueBufferIndex = 0;

    auto modelMatrix = feature.modelMatrix;
    auto translation = glm::vec3(1.0f, 0.0f, 0.0f);

    for (const auto& character : feature.text) {
      auto glyphVertexValueBuffer = glyphVertexValueBuffers[glyphValueBufferIndex];
      auto metalGlyphVertexFeature = static_cast<MetalGlyphVertexFeature*>(glyphVertexValueBuffer->contents());
      memcpy(&metalGlyphVertexFeature->modelMatrix, &modelMatrix, sizeof(simd::float4x4));

      commandEncoder->setVertexBuffer(glyphVertexValueBuffer, 0, 3);

      auto glyphFragmentValueBuffer = glyphFragmentValueBuffers[glyphValueBufferIndex];
      auto metalGlyphFragmentFeature = static_cast<MetalGlyphFragmentFeature*>(glyphFragmentValueBuffer->contents());
      memcpy(&metalGlyphFragmentFeature->position, &_glyphPositions.at(character), sizeof(simd::float2));

      commandEncoder->setFragmentBuffer(glyphFragmentValueBuffer, 0, 0);

      mesh->draw(*commandEncoder);

      modelMatrix = glm::translate(modelMatrix, translation);
      ++glyphValueBufferIndex;
    }

    ++colorValueBufferIndex;
  }

  commandEncoder->endEncoding();
}

void TextFeatureRenderer::ensureCameraBuffersCapacity(uint64_t maxId) {
  while (_cameraBuffers.size() < maxId + 1) {
    auto cameraBuffer = _context.device->newBuffer(sizeof(MetalCamera), MTL::ResourceStorageModeShared);
    _cameraBuffers.push_back(cameraBuffer);
    _colorValueBuffers.emplace_back();
    _glyphVertexValueBuffers.emplace_back();
    _glyphFragmentValueBuffers.emplace_back();
  }
}

}  // namespace linguine::render
