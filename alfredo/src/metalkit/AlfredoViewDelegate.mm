#import "AlfredoViewDelegate.h"

@implementation AlfredoViewDelegate

- (instancetype)initWithRenderer:(linguine::alfredo::MetalRenderer *)renderer {
  self = [super init];

  if (self) {
    _renderer = renderer;
  }

  return self;
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
  // No-op
}

- (void)drawInMTKView:(nonnull MTKView *)view {
  _renderer->drawInternal();
}

@end