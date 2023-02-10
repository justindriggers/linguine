#import "AlfredoViewDelegate.h"

@implementation AlfredoViewDelegate

- (instancetype)initWithRenderer:(linguine::render::MetalRenderer *)renderer {
  self = [super init];

  if (self) {
    _renderer = renderer;
  }

  return self;
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
  auto viewport = _renderer->getViewport();
  viewport->setSize(static_cast<uint16_t>(size.width), static_cast<uint16_t>(size.height));
}

- (void)drawInMTKView:(nonnull MTKView *)view {
  _renderer->doDraw();
}

@end