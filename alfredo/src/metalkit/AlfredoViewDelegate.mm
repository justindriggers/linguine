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
  _renderer->resize(static_cast<uint16_t>(size.width), static_cast<uint16_t>(size.height));
}

- (void)drawInMTKView:(nonnull MTKView *)view {
  _renderer->doDraw();
}

@end