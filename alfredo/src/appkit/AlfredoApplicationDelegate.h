#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <MetalRenderer.h>

#import "../metalkit/AlfredoViewDelegate.h"

@interface AlfredoApplicationDelegate : NSObject <NSApplicationDelegate> {
  @private
    MTKView *view;
    AlfredoViewDelegate *_viewDelegate;
}
@property(nonatomic, strong) NSWindow *window;
@property(nonatomic, retain) id<MTLDevice> device;
@property(nonatomic) linguine::render::MetalTextureLoader *metalTextureLoader;
@property(nonatomic) linguine::render::MetalRenderer *metalRenderer;
@end
