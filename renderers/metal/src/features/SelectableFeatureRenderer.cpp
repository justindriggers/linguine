#include "SelectableFeatureRenderer.h"

#include "renderer/features/SelectableFeature.h"

namespace linguine::render {

SelectableFeatureRenderer::SelectableFeatureRenderer(MetalRenderContext& context,
                                                     MeshRegistry& meshRegistry)
    : _context(context), _meshRegistry(meshRegistry) {
  const auto shaderSourceCode = R"(
      struct MetalCamera {
        metal::float4x4 viewProjectionMatrix;
      };

      struct MetalSelectableFeature {
        metal::float4x4 modelMatrix;
        uint2 entityId;
      };

      struct VertexOutput {
        float4 position [[position]];
        uint2 entityId;
      };

      VertexOutput vertex vertexWithId(uint index [[vertex_id]],
          const device float2* positions [[buffer(0)]],
          const constant MetalCamera& camera [[buffer(1)]],
          const constant MetalSelectableFeature& feature [[buffer(2)]]) {
        VertexOutput o;
        o.position = camera.viewProjectionMatrix * feature.modelMatrix * float4(positions[index], 0.0, 1.0);
        o.entityId = feature.entityId;
        return o;
      }

      uint2 fragment fragmentWithId(VertexOutput in [[stage_in]]) {
        return in.entityId;
      }
    )";

  NS::Error* error = nullptr;
  const auto library = context.device->newLibrary(NS::String::string(shaderSourceCode, NS::StringEncoding::UTF8StringEncoding), nullptr, &error);

  if (!library) {
    throw std::runtime_error(error->localizedDescription()->utf8String());
  }

  const auto vertexFunction = library->newFunction(NS::String::string("vertexWithId", NS::StringEncoding::UTF8StringEncoding));
  const auto fragmentFunction = library->newFunction(NS::String::string("fragmentWithId", NS::StringEncoding::UTF8StringEncoding));

  const auto renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
  renderPipelineDescriptor->setVertexFunction(vertexFunction);
  renderPipelineDescriptor->setFragmentFunction(fragmentFunction);
  renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatRG32Uint);
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

  _selectableRenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
  _selectableRenderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
  _selectableRenderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
  _selectableRenderPassDescriptor->depthAttachment()->setClearDepth(1.0f);
  _selectableRenderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionDontCare);

  depthStencilDescriptor->release();
  vertexFunction->release();
  fragmentFunction->release();
  renderPipelineDescriptor->release();
  library->release();
}

SelectableFeatureRenderer::~SelectableFeatureRenderer() {
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
  _selectableRenderPassDescriptor->release();
  _selectableDepthTexture->release();
  _selectableTexture->release();
}

bool SelectableFeatureRenderer::isRelevant(Renderable& renderable) {
  return renderable.hasFeature<SelectableFeature>();
}

void SelectableFeatureRenderer::draw(Camera& camera) {
  if (camera.getId() == 0) {
    _selectableRenderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
    _selectableRenderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(UINT32_MAX, UINT32_MAX, 0, 0));
  } else {
    _selectableRenderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionLoad);
  }

  auto commandEncoder = _context.commandBuffer->renderCommandEncoder(_selectableRenderPassDescriptor);

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
    valueBuffers.push_back(_context.device->newBuffer(sizeof(MetalSelectableFeature), MTL::ResourceStorageModeShared));
  }

  auto valueBufferIndex = 0;

  for (const auto& renderable : filteredRenderables) {
    auto feature = renderable->getFeature<SelectableFeature>();

    auto& mesh = _meshRegistry.get(feature.meshType);
    mesh->bind(*commandEncoder);

    auto valueBuffer = valueBuffers[valueBufferIndex++];
    auto metalSelectableFeature = static_cast<MetalSelectableFeature*>(valueBuffer->contents());

    memcpy(&metalSelectableFeature->modelMatrix, &feature.modelMatrix, sizeof(simd::float4x4));
    metalSelectableFeature->entityId = toUint2(feature.entityId);

    commandEncoder->setVertexBuffer(valueBuffer, 0, 2);
    mesh->draw(*commandEncoder);
  }

  commandEncoder->endEncoding();
}

void SelectableFeatureRenderer::resize(uint16_t width, uint16_t height) {
  if (_selectableTexture) {
    _selectableTexture->release();
  }

  const auto selectableTextureDescriptor = MTL::TextureDescriptor::alloc()->init();
  selectableTextureDescriptor->setTextureType(MTL::TextureType2D);
  selectableTextureDescriptor->setWidth(width);
  selectableTextureDescriptor->setHeight(height);
  selectableTextureDescriptor->setPixelFormat(MTL::PixelFormatRG32Uint);
  selectableTextureDescriptor->setUsage(MTL::TextureUsageRenderTarget);
  selectableTextureDescriptor->setStorageMode(MTL::StorageMode::StorageModeShared);

  _selectableTexture = _context.device->newTexture(selectableTextureDescriptor);
  _selectableRenderPassDescriptor->colorAttachments()->object(0)->setTexture(_selectableTexture);

  if (_selectableDepthTexture) {
    _selectableDepthTexture->release();
  }

  const auto selectableDepthTextureDescriptor = MTL::TextureDescriptor::alloc()->init();
  selectableDepthTextureDescriptor->setTextureType(MTL::TextureType2D);
  selectableDepthTextureDescriptor->setWidth(width);
  selectableDepthTextureDescriptor->setHeight(height);
  selectableDepthTextureDescriptor->setPixelFormat(MTL::PixelFormatDepth32Float);
  selectableDepthTextureDescriptor->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite);
  selectableDepthTextureDescriptor->setStorageMode(MTL::StorageMode::StorageModeMemoryless);

  _selectableDepthTexture = _context.device->newTexture(selectableDepthTextureDescriptor);
  _selectableRenderPassDescriptor->depthAttachment()->setTexture(_selectableDepthTexture);

  selectableTextureDescriptor->release();
  selectableDepthTextureDescriptor->release();
}

std::optional<uint64_t> SelectableFeatureRenderer::getEntityIdAt(float x, float y) const {
  auto region = MTL::Region::Make2D(
      static_cast<uint16_t>(x * _selectableTexture->width() - 1),
      static_cast<uint16_t>((1.0f - y) * _selectableTexture->height() - 1),
      1,
      1
  );

  simd::uint2 pixel = {UINT32_MAX, UINT32_MAX};
  _selectableTexture->getBytes(&pixel, _selectableTexture->bufferBytesPerRow(), region, 0);

  auto id = SelectableFeatureRenderer::toUint64(pixel);

  if (id != UINT64_MAX) {
    return std::make_optional(id);
  }

  return {};
}

void SelectableFeatureRenderer::ensureCameraBuffersCapacity(uint64_t maxId) {
  while (_cameraBuffers.size() < maxId + 1) {
    auto cameraBuffer = _context.device->newBuffer(sizeof(MetalCamera), MTL::ResourceStorageModeShared);
    _cameraBuffers.push_back(cameraBuffer);
    _valueBuffers.emplace_back();
  }
}

}  // namespace linguine::render
