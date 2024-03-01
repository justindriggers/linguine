#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <MetalRenderBackend.h>

#import "metalkit/AlfredoViewDelegate.h"

@interface AlfredoApplicationDelegate : NSObject <NSApplicationDelegate> {
  @private
    AlfredoViewDelegate *_viewDelegate;
}
@property(nonatomic, strong) NSWindow *window;
@property(nonatomic, strong) MTKView *view;
@property(nonatomic, retain) id<MTLDevice> device;
@property(nonatomic) std::shared_ptr<LifecycleManager> lifecycleManager;
@end
