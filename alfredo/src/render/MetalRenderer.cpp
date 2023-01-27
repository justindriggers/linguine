#include "MetalRenderer.h"

#include <iostream>

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION

#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

class linguine::alfredo::MetalRendererImpl : public linguine::alfredo::MetalRenderer {
  public:
    explicit MetalRendererImpl(MTK::View& view) : _view(view) {
      _device = _view.device();

      std::cout << _device->name()->cString(NS::StringEncoding::UTF8StringEncoding) << std::endl;

      _commandQueue = _device->newCommandQueue();
    }

    ~MetalRendererImpl() override {
      _device->release();
    }

    void draw() override {
      _view.draw();
    }

    void drawInternal() override;

  private:
    MTK::View& _view;
    MTL::Device* _device = nullptr;
    MTL::CommandQueue* _commandQueue = nullptr;
};

void linguine::alfredo::MetalRendererImpl::drawInternal() {
  auto pool = NS::AutoreleasePool::alloc()->init();

  auto commandBuffer = _commandQueue->commandBuffer();

  auto renderCommandEncoder = commandBuffer->renderCommandEncoder(_view.currentRenderPassDescriptor());
  // TODO Draw stuff
  renderCommandEncoder->endEncoding();

  commandBuffer->presentDrawable(_view.currentDrawable());
  commandBuffer->commit();

  pool->release();
}

linguine::alfredo::MetalRenderer* linguine::alfredo::MetalRenderer::create(MTK::View& view) {
  return new MetalRendererImpl(view);
}
