#import "ScampiViewDelegate.h"

#import "../platform/NSLogger.h"

@implementation ScampiViewDelegate {
  linguine::Engine* _engine;
  linguine::scampi::NSLogger logger;
}

- (instancetype)initWithEngine:(nonnull linguine::Engine *)engine {
  self = [super init];

  if (self) {
    _engine = engine;
  }

  return self;
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
  @autoreleasepool {
    NSString *sizeStr = NSStringFromCGSize(size);
    logger.log(sizeStr);
  }
}

- (void)drawInMTKView:(nonnull MTKView *)view {
  _engine->tick();
}

@end