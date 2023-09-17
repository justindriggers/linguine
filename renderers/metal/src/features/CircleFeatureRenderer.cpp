#include "CircleFeatureRenderer.h"

#include "renderer/features/CircleFeature.h"

namespace linguine::render {

CircleFeatureRenderer::CircleFeatureRenderer(MetalRenderContext& context,
                                             MeshRegistry& meshRegistry)
    : _context(context), _meshRegistry(meshRegistry) {
  const auto shaderSourceCode = R"(
      struct MetalCamera {
        metal::float4x4 viewProjectionMatrix;
      };

      struct MetalColoredFeature {
        metal::float4x4 modelMatrix;
        float3 color;
      };

      struct VertexOutput {
        float4 position [[position]];
        float3 color;
        float2 uv;
      };

      VertexOutput vertex vertexColored(uint index [[vertex_id]],
          const device float2* positions [[buffer(0)]],
          const constant MetalCamera& camera [[buffer(1)]],
          const constant MetalColoredFeature& feature [[buffer(2)]]) {
        VertexOutput o;
        o.position = camera.viewProjectionMatrix * feature.modelMatrix * float4(positions[index], 0.0, 1.0);
        o.color = feature.color;
        o.uv = positions[index];
        return o;
      }

      float4 fragment fragmentColored(VertexOutput in [[stage_in]]) {
        float distance = metal::length(in.uv);
        float delta = metal::fwidth(distance);
        float alpha = 1.0 - metal::smoothstep(0.5 - delta, 0.5, distance);

        return float4(in.color, alpha);
      }
    )";

  NS::Error* error = nullptr;
  const auto library = context.device->newLibrary(NS::String::string(shaderSourceCode, NS::StringEncoding::UTF8StringEncoding), nullptr, &error);

  if (!library) {
    throw std::runtime_error(error->localizedDescription()->utf8String());
  }

  const auto vertexFunction = library->newFunction(NS::String::string("vertexColored", NS::StringEncoding::UTF8StringEncoding));
  const auto fragmentFunction = library->newFunction(NS::String::string("fragmentColored", NS::StringEncoding::UTF8StringEncoding));

  const auto renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
  renderPipelineDescriptor->setVertexFunction(vertexFunction);
  renderPipelineDescriptor->setFragmentFunction(fragmentFunction);

  auto colorAttachment = renderPipelineDescriptor->colorAttachments()->object(0);
  colorAttachment->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
  colorAttachment->setBlendingEnabled(true);
  colorAttachment->setRgbBlendOperation(MTL::BlendOperation::BlendOperationAdd);
  colorAttachment->setAlphaBlendOperation(MTL::BlendOperation::BlendOperationMax);
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

CircleFeatureRenderer::~CircleFeatureRenderer() {
  for (const auto& cameraValueBuffers : _valueBuffers) {
    for (const auto& valueBuffer : cameraValueBuffers) {
      valueBuffer->release();
    }
  }

  for (const auto& cameraBuffer : _cameraBuffers) {
    cameraBuffer->release();
  }

  _pipelineState->release();
  _depthState->release();
}

bool CircleFeatureRenderer::isRelevant(Renderable& renderable) {
  return renderable.hasFeature<CircleFeature>();
}

void CircleFeatureRenderer::draw(Camera& camera) {
  auto commandEncoder = _context.commandBuffer->renderCommandEncoder(_context.coloredRenderPassDescriptor);

  commandEncoder->setRenderPipelineState(_pipelineState);
  commandEncoder->setDepthStencilState(_depthState);

  ensureCameraBuffersCapacity(camera.getId());

  auto cameraBuffer = _cameraBuffers[camera.getId()];
  auto metalCamera = static_cast<MetalCamera*>(cameraBuffer->contents());
  memcpy(&metalCamera->viewProjectionMatrix, &camera.viewProjectionMatrix, sizeof(simd::float4x4));
  commandEncoder->setVertexBuffer(cameraBuffer, 0, 1);

  auto filteredRenderables = std::vector<Renderable*>();

  for (const auto& renderable : getRenderables()) {
    if (renderable.second->getLayer() == camera.layer && renderable.second->isEnabled()) {
      filteredRenderables.push_back(renderable.second);
    }
  }

  auto& valueBuffers = _valueBuffers[camera.getId()];

  while (valueBuffers.size() < filteredRenderables.size()) {
    valueBuffers.push_back(_context.device->newBuffer(sizeof(MetalCircleFeature), MTL::ResourceStorageModeShared));
  }

  auto valueBufferIndex = 0;

  for (const auto& renderable : filteredRenderables) {
    auto feature = renderable->getFeature<CircleFeature>();

    auto& mesh = _meshRegistry.get(MeshType::Quad);
    mesh->bind(*commandEncoder);

    auto valueBuffer = valueBuffers[valueBufferIndex++];
    auto metalColoredFeature = static_cast<MetalCircleFeature*>(valueBuffer->contents());

    memcpy(&metalColoredFeature->modelMatrix, &feature.modelMatrix, sizeof(simd::float4x4));
    memcpy(&metalColoredFeature->color, &feature.color, sizeof(simd::float3));

    commandEncoder->setVertexBuffer(valueBuffer, 0, 2);
    mesh->draw(*commandEncoder);
  }

  commandEncoder->endEncoding();
}

void CircleFeatureRenderer::ensureCameraBuffersCapacity(uint64_t maxId) {
  while (_cameraBuffers.size() < maxId + 1) {
    auto cameraBuffer = _context.device->newBuffer(sizeof(MetalCamera), MTL::ResourceStorageModeShared);
    _cameraBuffers.push_back(cameraBuffer);
    _valueBuffers.emplace_back();
  }
}

}  // namespace linguine::render
