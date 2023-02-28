#include "SelectableFeatureRenderer.h"

#include "renderer/features/SelectableFeature.h"

namespace linguine::render {

SelectableFeatureRenderer::SelectableFeatureRenderer(MetalRenderContext& context,
                                                     Camera& camera,
                                                     MeshRegistry& meshRegistry)
    : _context(context), _camera(camera), _meshRegistry(meshRegistry) {
  _cameraBuffer = context.device->newBuffer(sizeof(MetalCamera), MTL::ResourceStorageModeShared);

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
  _selectableRenderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
  _selectableRenderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(UINT32_MAX, UINT32_MAX, 0, 0));
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
  for (const auto& valueBuffer : _valueBuffers) {
    valueBuffer->release();
  }

  _cameraBuffer->release();
  _pipelineState->release();
  _depthState->release();
  _selectableRenderPassDescriptor->release();
  _selectableDepthTexture->release();
  _selectableTexture->release();
}

bool SelectableFeatureRenderer::isRelevant(Renderable& renderable) {
  return renderable.hasFeature<SelectableFeature>();
}

void SelectableFeatureRenderer::draw() {
  auto commandEncoder = _context.commandBuffer->renderCommandEncoder(_selectableRenderPassDescriptor);

  commandEncoder->setRenderPipelineState(_pipelineState);
  commandEncoder->setDepthStencilState(_depthState);

  auto metalCamera = static_cast<MetalCamera*>(_cameraBuffer->contents());
  memcpy(&metalCamera->viewProjectionMatrix, &_camera.viewProjectionMatrix, sizeof(simd::float4x4));
  _cameraBuffer->didModifyRange(NS::Range::Make(0, sizeof(MetalCamera)));
  commandEncoder->setVertexBuffer(_cameraBuffer, 0, 1);

  const auto renderables = getRenderables();

  while (_valueBuffers.size() < renderables.size()) {
    _valueBuffers.push_back(_context.device->newBuffer(sizeof(MetalSelectableFeature), MTL::ResourceStorageModeShared));
  }

  for (int i = 0; i < renderables.size(); ++i) {
    auto renderable = renderables[i];

    if (renderable && renderable->isEnabled()) {
      auto feature = renderable->getFeature<SelectableFeature>();

      auto& mesh = _meshRegistry.get(feature->meshType);
      mesh->bind(*commandEncoder);

      auto valueBuffer = _valueBuffers[i];
      auto metalSelectableFeature = static_cast<MetalSelectableFeature*>(valueBuffer->contents());

      memcpy(&metalSelectableFeature->modelMatrix, &feature->modelMatrix, sizeof(simd::float4x4));
      metalSelectableFeature->entityId = toUint2(feature->entityId);
      valueBuffer->didModifyRange(NS::Range::Make(0, sizeof(MetalSelectableFeature)));

      commandEncoder->setVertexBuffer(valueBuffer, 0, 2);
      mesh->draw(*commandEncoder);
    }
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
}

std::optional<uint64_t> SelectableFeatureRenderer::getEntityIdAt(float x, float y) const {
  auto region = MTL::Region::Make2D(
      static_cast<uint16_t>(x * _selectableTexture->width()),
      static_cast<uint16_t>((1.0f - y) * _selectableTexture->height()),
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

}  // namespace linguine::render
