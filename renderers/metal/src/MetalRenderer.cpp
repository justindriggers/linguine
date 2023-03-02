#include "MetalRenderer.h"

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION

#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

#include <renderer/features/FeatureRenderer.h>

#include "features/ColoredFeatureRenderer.h"
#include "features/SelectableFeatureRenderer.h"

namespace linguine::render {

class MetalRendererImpl : public MetalRenderer {
  public:
    explicit MetalRendererImpl(MTK::View& view, bool autoDraw)
        : _context{}, _view(view), _autoDraw(autoDraw) {
      _view.setColorPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
      _view.setDepthStencilPixelFormat(MTL::PixelFormatDepth32Float);
      _view.setClearColor(MTL::ClearColor::Make(0.0f, 0.0f, 0.0f, 1.0f));
      _view.setClearDepth(1.0f);

      _context.device = _view.device();
      _context.commandQueue = _context.device->newCommandQueue();

      _meshRegistry = std::make_unique<MeshRegistry>(*_context.device);

      _selectableFeatureRenderer = new SelectableFeatureRenderer(_context, getCamera(), *_meshRegistry);

      _features.push_back(std::make_unique<ColoredFeatureRenderer>(_context, getCamera(), *_meshRegistry));
      _features.push_back(std::unique_ptr<SelectableFeatureRenderer>(_selectableFeatureRenderer));
      _features.shrink_to_fit();
    }

    ~MetalRendererImpl() override {
      _context.commandQueue->release();
      _context.device->release();
    }

    void draw() override;

    void doDraw() override;

    [[nodiscard]] std::optional<uint64_t> getEntityIdAt(float x, float y) const override;

  protected:
    [[nodiscard]] const std::vector<std::unique_ptr<FeatureRenderer>>& getFeatures() const override {
      return _features;
    }

  private:
    MTK::View& _view;
    bool _autoDraw;

    MetalRenderContext _context;
    std::unique_ptr<MeshRegistry> _meshRegistry;

    std::vector<std::unique_ptr<FeatureRenderer>> _features;
    SelectableFeatureRenderer* _selectableFeatureRenderer = nullptr;
};

void MetalRendererImpl::draw() {
  if (_autoDraw) {
    doDraw();
  } else {
    _view.draw();
  }
}

void MetalRendererImpl::doDraw() {
  auto pool = NS::AutoreleasePool::alloc()->init();

  _context.commandBuffer = _context.commandQueue->commandBuffer();
  _context.coloredRenderPassDescriptor = _view.currentRenderPassDescriptor();

  for (const auto& feature : getFeatures()) {
    feature->draw();
  }

  _context.commandBuffer->presentDrawable(_view.currentDrawable());
  _context.commandBuffer->commit();

  pool->release();
}

std::optional<uint64_t> MetalRendererImpl::getEntityIdAt(float x, float y) const {
  return _selectableFeatureRenderer->getEntityIdAt(x, y);
}

MetalRenderer* MetalRenderer::create(MTK::View& view, bool autoDraw) {
  return new MetalRendererImpl(view, autoDraw);
}

} // namespace linguine::render
