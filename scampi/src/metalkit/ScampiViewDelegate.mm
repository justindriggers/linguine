#import "ScampiViewDelegate.h"

#import "../platform/NSLogger.h"

@implementation ScampiViewDelegate {
  std::shared_ptr<linguine::Engine> _engine;
  std::shared_ptr<linguine::Renderer> _renderer;
}

- (instancetype)initWithEngine:(std::shared_ptr<linguine::Engine>&)engine
                      renderer:(std::shared_ptr<linguine::Renderer>&)renderer {
  self = [super init];

  if (self) {
    _engine = engine;
    _renderer = renderer;
  }

  return self;
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
  auto viewport = _renderer->getViewport();
  viewport->setSize(static_cast<uint16_t>(size.width), static_cast<uint16_t>(size.height));
}

- (void)drawInMTKView:(nonnull MTKView *)view {
  _engine->tick();
}

@end