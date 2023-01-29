#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <MetalRenderer.h>

#import "../render/MTKRenderer.h"

@interface AlfredoApplicationDelegate : NSObject <NSApplicationDelegate> {
  @private
    MTKView *view;
}
@property(nonatomic, strong) NSWindow *window;
@property(nonatomic, retain) id<MTLDevice> device;
@property(nonatomic) linguine::alfredo::MTKRenderer *mtkRenderer;
@property(nonatomic) linguine::render::MetalRenderer *metalRenderer;
@end
