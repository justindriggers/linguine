#include "MetalRenderBackend.h"

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION

#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

#include <renderer/features/FeatureRenderer.h>

#include "features/CircleFeatureRenderer.h"
#include "features/ColoredFeatureRenderer.h"
#include "features/ProgressFeatureRenderer.h"
#include "features/SelectableFeatureRenderer.h"
#include "features/TextFeatureRenderer.h"

namespace linguine::render {

class MetalRenderBackendImpl : public MetalRenderBackend {
  public:
    MetalRenderBackendImpl(MTK::View& view, std::unique_ptr<MetalTextureLoader> textureLoader)
        : _context{}, _view(view), _textureLoader(std::move(textureLoader)) {
      _view.setColorPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
      _view.setDepthStencilPixelFormat(MTL::PixelFormatDepth32Float);

      _context.device = _view.device();
      _context.commandQueue = _context.device->newCommandQueue();

      _meshRegistry = std::make_unique<MeshRegistry>(*_context.device);

      _selectableFeatureRenderer = new SelectableFeatureRenderer(_context, *_meshRegistry);

      _features.push_back(std::make_unique<ColoredFeatureRenderer>(_context, *_meshRegistry));
      _features.push_back(std::make_unique<ProgressFeatureRenderer>(_context, *_meshRegistry));
      _features.push_back(std::unique_ptr<SelectableFeatureRenderer>(_selectableFeatureRenderer));
      _features.push_back(std::make_unique<TextFeatureRenderer>(_context, *_meshRegistry, *_textureLoader));
      _features.push_back(std::make_unique<CircleFeatureRenderer>(_context, *_meshRegistry));
      _features.shrink_to_fit();
    }

    ~MetalRenderBackendImpl() override {
      _context.commandQueue->release();
      _context.device->release();
    }

    void draw(const std::vector<std::unique_ptr<Camera>>& cameras) override;

    void resize(uint16_t width, uint16_t height) override;

    void reset() override;

    [[nodiscard]] std::optional<uint64_t> getEntityIdAt(float x, float y) const override;

  protected:
    [[nodiscard]] const std::vector<std::unique_ptr<FeatureRenderer>>& getFeatures() const override {
      return _features;
    }

  private:
    MTK::View& _view;
    std::unique_ptr<MetalTextureLoader> _textureLoader;

    MetalRenderContext _context;
    std::unique_ptr<MeshRegistry> _meshRegistry;

    std::vector<std::unique_ptr<FeatureRenderer>> _features;
    SelectableFeatureRenderer* _selectableFeatureRenderer = nullptr;
    bool _isFirstFrame = true;
};

void MetalRenderBackendImpl::draw(const std::vector<std::unique_ptr<Camera>>& cameras) {
  auto pool = NS::AutoreleasePool::alloc()->init();

  _context.commandBuffer = _context.commandQueue->commandBuffer();
  _context.coloredRenderPassDescriptor = _view.currentRenderPassDescriptor();

  for (const auto& feature : getFeatures()) {
    feature->onFrameBegin();
  }

  for (const auto& camera : cameras) {
    if (camera->clearColor) {
      auto clearColor = *camera->clearColor;
      _context.coloredRenderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
      _context.coloredRenderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(clearColor.r, clearColor.g, clearColor.b, 1.0f));
    } else {
      _context.coloredRenderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionLoad);
    }

    _context.coloredRenderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);

    _context.coloredRenderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
    _context.coloredRenderPassDescriptor->depthAttachment()->setClearDepth(1.0f);
    _context.coloredRenderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionStore);

    for (const auto& feature : getFeatures()) {
      feature->draw(*camera);

      _context.coloredRenderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionLoad);
      _context.coloredRenderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionLoad);
    }
  }

  _context.commandBuffer->presentDrawable(_view.currentDrawable());
  _context.commandBuffer->commit();
  _context.commandBuffer->waitUntilCompleted();

  pool->release();

  _isFirstFrame = false;
}

void MetalRenderBackendImpl::resize(uint16_t width, uint16_t height) {
  for (const auto& feature : getFeatures()) {
    feature->resize(width, height);
  }
}

void MetalRenderBackendImpl::reset() {
  _isFirstFrame = true;

  for (const auto& feature : getFeatures()) {
    feature->reset();
  }
}

std::optional<uint64_t> MetalRenderBackendImpl::getEntityIdAt(float x, float y) const {
  if (_isFirstFrame) {
    return {};
  }

  return _selectableFeatureRenderer->getEntityIdAt(x, y);
}

std::unique_ptr<MetalRenderBackend> MetalRenderBackend::create(MTK::View& view, std::unique_ptr<MetalTextureLoader> textureLoader) {
  return std::make_unique<MetalRenderBackendImpl>(view, std::move(textureLoader));
}

} // namespace linguine::render
