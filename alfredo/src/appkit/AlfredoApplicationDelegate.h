#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#import "../render/MetalRenderer.h"

@interface AlfredoApplicationDelegate : NSObject <NSApplicationDelegate> {
  @private
    MTKView *view;
}
@property(nonatomic, strong) NSWindow *window;
@property(nonatomic) id<MTLDevice> device;
@property(nonatomic) linguine::alfredo::MetalRenderer *renderer;
@end
