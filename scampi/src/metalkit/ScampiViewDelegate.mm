#import "ScampiViewDelegate.h"

@implementation ScampiViewDelegate {
  std::shared_ptr<Engine> _engine;
}

- (instancetype)initWithEngine:(const std::shared_ptr<Engine>&)engine {
  self = [super init];

  if (self) {
    _engine = engine;
  }

  return self;
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
  _engine->get<Renderer>().resize(static_cast<uint16_t>(size.width), static_cast<uint16_t>(size.height));
}

- (void)drawInMTKView:(nonnull MTKView *)view {
  _engine->tick();
}

@end