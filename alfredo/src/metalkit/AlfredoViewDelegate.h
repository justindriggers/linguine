#import <MetalKit/MetalKit.h>

#import <MetalRenderer.h>

@interface AlfredoViewDelegate : NSObject <MTKViewDelegate> {
  @private
    linguine::render::MetalRenderer * _renderer;
}

- (instancetype)initWithRenderer:(linguine::render::MetalRenderer *)renderer;

@end