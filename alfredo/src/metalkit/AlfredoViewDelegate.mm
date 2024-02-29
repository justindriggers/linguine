#import "AlfredoViewDelegate.h"

@implementation AlfredoViewDelegate

- (instancetype)initWithEngine:(std::unique_ptr<Engine>)engine {
  self = [super init];

  if (self) {
    _engine = std::move(engine);
  }

  return self;
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
  _engine->get<linguine::Renderer>().resize(static_cast<uint16_t>(size.width), static_cast<uint16_t>(size.height));
}

- (void)drawInMTKView:(nonnull MTKView *)view {
  _engine->tick();
}

@end