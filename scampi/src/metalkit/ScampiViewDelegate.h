#import <MetalKit/MetalKit.h>

#import <Engine.h>

using namespace linguine;

@interface ScampiViewDelegate : NSObject <MTKViewDelegate>

- (instancetype)initWithEngine:(const std::shared_ptr<Engine>&)engine;

@end
