#import <MetalKit/MetalKit.h>

#import <Engine.h>

@interface ScampiViewDelegate : NSObject <MTKViewDelegate>

- (instancetype)initWithEngine:(linguine::Engine *)engine;

@end