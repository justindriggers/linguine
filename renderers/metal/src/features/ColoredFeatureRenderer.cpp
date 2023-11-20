#include "ColoredFeatureRenderer.h"

#include "renderer/features/ColoredFeature.h"

namespace linguine::render {

ColoredFeatureRenderer::ColoredFeatureRenderer(MetalRenderContext& context,
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
      };

      VertexOutput vertex vertexColored(uint index [[vertex_id]],
          const device float2* positions [[buffer(0)]],
          const constant MetalCamera& camera [[buffer(1)]],
          const constant MetalColoredFeature& feature [[buffer(2)]]) {
        VertexOutput o;
        o.position = camera.viewProjectionMatrix * feature.modelMatrix * float4(positions[index], 0.0, 1.0);
        o.color = feature.color;
        return o;
      }

      float4 fragment fragmentColored(VertexOutput in [[stage_in]]) {
        return float4(in.color, 1.0);
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
  renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
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

ColoredFeatureRenderer::~ColoredFeatureRenderer() {
  reset();

  _pipelineState->release();
  _depthState->release();
}

bool ColoredFeatureRenderer::isRelevant(Renderable& renderable) {
  return renderable.hasFeature<ColoredFeature>();
}

void ColoredFeatureRenderer::draw(Camera& camera) {
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

  auto& valueBuffers = _valueBuffers[camera.getId()];

  while (valueBuffers.size() < filteredRenderables.size()) {
    valueBuffers.push_back(_context.device->newBuffer(sizeof(MetalColoredFeature), MTL::ResourceStorageModeShared));
  }

  auto valueBufferIndex = 0;

  for (const auto& renderable : filteredRenderables) {
    auto feature = renderable->getFeature<ColoredFeature>();

    auto& mesh = _meshRegistry.get(feature.meshType);
    mesh->bind(*commandEncoder);

    auto valueBuffer = valueBuffers[valueBufferIndex++];
    auto metalColoredFeature = static_cast<MetalColoredFeature*>(valueBuffer->contents());

    memcpy(&metalColoredFeature->modelMatrix, &feature.modelMatrix, sizeof(simd::float4x4));
    memcpy(&metalColoredFeature->color, &feature.color, sizeof(simd::float3));

    commandEncoder->setVertexBuffer(valueBuffer, 0, 2);
    mesh->draw(*commandEncoder);
  }

  commandEncoder->endEncoding();
}

void ColoredFeatureRenderer::reset() {
  for (const auto& cameraValueBuffers : _valueBuffers) {
    for (const auto& valueBuffer : cameraValueBuffers.second) {
      valueBuffer->release();
    }
  }

  _valueBuffers.clear();

  for (const auto& cameraBuffer : _cameraBuffers) {
    cameraBuffer.second->release();
  }

  _cameraBuffers.clear();
}

void ColoredFeatureRenderer::ensureCameraBuffers(uint64_t cameraId) {
  if (_cameraBuffers.find(cameraId) == _cameraBuffers.end()) {
    auto cameraBuffer = _context.device->newBuffer(sizeof(MetalCamera), MTL::ResourceStorageModeShared);
    _cameraBuffers.emplace(cameraId, cameraBuffer);
    _valueBuffers.emplace(cameraId, std::vector<MTL::Buffer*>());
  }
}

}  // namespace linguine::render
