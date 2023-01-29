#include "MetalRenderer.h"

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION

#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

class linguine::render::MetalRendererImpl : public linguine::render::MetalRenderer {
  public:
    explicit MetalRendererImpl(MTK::View& view) : _view(view) {
      _view.setColorPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
      _view.setClearColor(MTL::ClearColor::Make(0.0f, 0.0f, 1.0f, 1.0f));

      _device = _view.device();

      _commandQueue = _device->newCommandQueue();
    }

    ~MetalRendererImpl() override {
      _device->release();
    }

    void draw() override;

  private:
    MTK::View& _view;
    MTL::Device* _device = nullptr;
    MTL::CommandQueue* _commandQueue = nullptr;
};

void linguine::render::MetalRendererImpl::draw() {
  auto pool = NS::AutoreleasePool::alloc()->init();

  auto commandBuffer = _commandQueue->commandBuffer();

  auto renderCommandEncoder = commandBuffer->renderCommandEncoder(_view.currentRenderPassDescriptor());
  // TODO Draw stuff
  renderCommandEncoder->endEncoding();

  commandBuffer->presentDrawable(_view.currentDrawable());
  commandBuffer->commit();

  pool->release();
}

linguine::render::MetalRenderer* linguine::render::MetalRenderer::create(MTK::View& view) {
  return new MetalRendererImpl(view);
}
