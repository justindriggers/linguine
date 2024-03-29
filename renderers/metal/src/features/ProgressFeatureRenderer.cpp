#include "ProgressFeatureRenderer.h"

#include "renderer/features/ProgressFeature.h"

namespace linguine::render {

ProgressFeatureRenderer::ProgressFeatureRenderer(MetalRenderContext& context,
                                                 MeshRegistry& meshRegistry)
    : _context(context), _meshRegistry(meshRegistry) {
  const auto shaderSourceCode = R"(
      struct MetalCamera {
        metal::float4x4 viewProjectionMatrix;
      };

      struct MetalProgressVertexFeature {
        metal::float4x4 modelMatrix;
      };

      struct VertexOutput {
        float4 position [[position]];
        float x;
      };

      struct MetalProgressFragmentFeature {
        float4 backgroundColor;
        float3 color;
        float progress;
      };

      VertexOutput vertex vertexProgress(uint index [[vertex_id]],
          const device float2* positions [[buffer(0)]],
          const constant MetalCamera& camera [[buffer(1)]],
          const constant MetalProgressVertexFeature& feature [[buffer(2)]]) {
        VertexOutput o;
        o.position = camera.viewProjectionMatrix * feature.modelMatrix * float4(positions[index], 0.0, 1.0);
        o.x = positions[index].x + 0.5;
        return o;
      }

      float4 fragment fragmentProgress(VertexOutput in [[stage_in]],
          const constant MetalProgressFragmentFeature& feature [[buffer(0)]]) {
        float4 result = in.x < feature.progress ? float4(feature.color, 1.0) : feature.backgroundColor;

        if (result.a > 0.0) {
          return result;
        }

        metal::discard_fragment();
      }
    )";

  NS::Error* error = nullptr;
  const auto library = context.device->newLibrary(NS::String::string(shaderSourceCode, NS::StringEncoding::UTF8StringEncoding), nullptr, &error);

  if (!library) {
    throw std::runtime_error(error->localizedDescription()->utf8String());
  }

  const auto vertexFunction = library->newFunction(NS::String::string("vertexProgress", NS::StringEncoding::UTF8StringEncoding));
  const auto fragmentFunction = library->newFunction(NS::String::string("fragmentProgress", NS::StringEncoding::UTF8StringEncoding));

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

ProgressFeatureRenderer::~ProgressFeatureRenderer() {
  reset();

  _pipelineState->release();
  _depthState->release();
}

bool ProgressFeatureRenderer::isRelevant(Renderable& renderable) {
  return renderable.hasFeature<ProgressFeature>();
}

void ProgressFeatureRenderer::draw(Camera& camera) {
  auto commandEncoder = _context.commandBuffer->renderCommandEncoder(_context.coloredRenderPassDescriptor);

  commandEncoder->setRenderPipelineState(_pipelineState);
  commandEncoder->setDepthStencilState(_depthState);

  ensureCameraBuffers(camera.getId());

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

  auto& vertexFeatureBuffers = _vertexFeatureBuffers[camera.getId()];

  while (vertexFeatureBuffers.size() < filteredRenderables.size()) {
    vertexFeatureBuffers.push_back(_context.device->newBuffer(sizeof(MetalProgressVertexFeature), MTL::ResourceStorageModeShared));
  }

  auto& fragmentFeatureBuffers = _fragmentFeatureBuffers[camera.getId()];

  while (fragmentFeatureBuffers.size() < filteredRenderables.size()) {
    fragmentFeatureBuffers.push_back(_context.device->newBuffer(sizeof(MetalProgressFragmentFeature), MTL::ResourceStorageModeShared));
  }

  auto valueBufferIndex = 0;

  for (const auto& renderable : filteredRenderables) {
    auto feature = renderable->getFeature<ProgressFeature>();

    auto& mesh = _meshRegistry.get(feature.meshType);
    mesh->bind(*commandEncoder);

    auto vertexFeatureBuffer = vertexFeatureBuffers[valueBufferIndex];
    auto metalVertexProgressFeature = static_cast<MetalProgressVertexFeature*>(vertexFeatureBuffer->contents());
    memcpy(&metalVertexProgressFeature->modelMatrix, &feature.modelMatrix, sizeof(simd::float4x4));

    auto fragmentFeatureBuffer = fragmentFeatureBuffers[valueBufferIndex];
    auto metalFragmentProgressFeature = static_cast<MetalProgressFragmentFeature*>(fragmentFeatureBuffer->contents());

    memcpy(&metalFragmentProgressFeature->backgroundColor, &feature.backgroundColor, sizeof(simd::float4));
    memcpy(&metalFragmentProgressFeature->color, &feature.color, sizeof(simd::float3));
    metalFragmentProgressFeature->progress = feature.progress;

    commandEncoder->setVertexBuffer(vertexFeatureBuffer, 0, 2);
    commandEncoder->setFragmentBuffer(fragmentFeatureBuffer, 0, 0);
    mesh->draw(*commandEncoder);

    valueBufferIndex++;
  }

  commandEncoder->endEncoding();
}

void ProgressFeatureRenderer::reset() {
  for (const auto& cameraVertexFeatureBuffers : _vertexFeatureBuffers) {
    for (const auto& vertexFeatureBuffer : cameraVertexFeatureBuffers.second) {
      vertexFeatureBuffer->release();
    }
  }

  _vertexFeatureBuffers.clear();

  for (const auto& cameraFragmentFeatureBuffers : _fragmentFeatureBuffers) {
    for (const auto& fragmentFeatureBuffer : cameraFragmentFeatureBuffers.second) {
      fragmentFeatureBuffer->release();
    }
  }

  _fragmentFeatureBuffers.clear();

  for (const auto& cameraBuffer : _cameraBuffers) {
    cameraBuffer.second->release();
  }

  _cameraBuffers.clear();
}

void ProgressFeatureRenderer::ensureCameraBuffers(uint64_t cameraId) {
  if (_cameraBuffers.find(cameraId) == _cameraBuffers.end()) {
    auto cameraBuffer = _context.device->newBuffer(sizeof(MetalCamera), MTL::ResourceStorageModeShared);
    _cameraBuffers.emplace(cameraId, cameraBuffer);
    _vertexFeatureBuffers.emplace(cameraId, std::vector<MTL::Buffer*>());
    _fragmentFeatureBuffers.emplace(cameraId, std::vector<MTL::Buffer*>());
  }
}

}  // namespace linguine::render
