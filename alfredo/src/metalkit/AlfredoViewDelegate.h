#import <MetalKit/MetalKit.h>

#import "../render/MetalRenderer.h"

@interface AlfredoViewDelegate : NSObject <MTKViewDelegate> {
  @private
    linguine::alfredo::MetalRenderer * _renderer;
}

- (instancetype)initWithRenderer:(linguine::alfredo::MetalRenderer *)renderer;

@end