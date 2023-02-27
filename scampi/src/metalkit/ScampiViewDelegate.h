#import <MetalKit/MetalKit.h>

#import <Engine.h>
#import <MetalRenderer.h>

@interface ScampiViewDelegate : NSObject <MTKViewDelegate>

- (instancetype)initWithEngine:(std::shared_ptr<linguine::Engine>&)engine
                      renderer:(std::shared_ptr<linguine::Renderer>&)renderer;

@end