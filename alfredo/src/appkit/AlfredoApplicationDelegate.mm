#import "AlfredoApplicationDelegate.h"

#import "../platform/MacMetalTextureLoader.h"

@implementation AlfredoApplicationDelegate

- (void)closeWindow {
  @autoreleasepool {
    [[NSApp mainWindow] close];
  }
}

- (void)createMenuBar {
  @autoreleasepool {
    NSMenu *mainMenu = [[NSMenu alloc] init];
    [NSApp setMainMenu:mainMenu];

    NSMenuItem *appMenuItem = [[NSMenuItem alloc] init];
    NSMenu *appMenu = [[NSMenu alloc] init];

    NSString *appName =
        [[NSRunningApplication currentApplication] localizedName];
    [appMenu addItemWithTitle:[NSString stringWithFormat:@"Quit %@", appName]
                       action:@selector(terminate:)
                keyEquivalent:@"q"];

    [appMenuItem setSubmenu:appMenu];

    NSMenuItem *windowMenuItem = [[NSMenuItem alloc] init];
    NSMenu *windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];

    [windowMenu addItemWithTitle:@"Close window"
                          action:@selector(closeWindow)
                   keyEquivalent:@"w"];

    [windowMenuItem setSubmenu:windowMenu];

    [mainMenu addItem:appMenuItem];
    [mainMenu addItem:windowMenuItem];
  }
}

- (void)applicationWillFinishLaunching:(NSNotification *)notification {
  @autoreleasepool {
    [self createMenuBar];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
  }
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
  @autoreleasepool {
    NSRect frame = CGRectMake(256.0, 256.0, 540.0, 900.0);

    self.window = [[NSWindow alloc]
        initWithContentRect:frame
                  styleMask:NSWindowStyleMaskClosable | NSWindowStyleMaskTitled
                    backing:NSBackingStoreBuffered
                      defer:NO];

    self.device = MTLCreateSystemDefaultDevice();
    view = [[MTKView alloc] initWithFrame:frame
                                   device:self.device];
    [view setPaused:YES];
    [view setEnableSetNeedsDisplay:NO];
    ((CAMetalLayer*)[view layer]).displaySyncEnabled = NO;

    auto mtkTextureLoader = [[MTKTextureLoader alloc] initWithDevice:self.device];
    self.metalTextureLoader = new linguine::alfredo::MacMetalTextureLoader(mtkTextureLoader);
    self.metalRenderer = linguine::render::MetalRenderer::create(*(__bridge MTK::View*)view, false, *self.metalTextureLoader);

    _viewDelegate = [[AlfredoViewDelegate alloc] initWithRenderer:self.metalRenderer];

    [_viewDelegate mtkView:view drawableSizeWillChange:view.bounds.size];
    [view setDelegate:_viewDelegate];

    [self.window setContentView:view];
    [self.window setTitle:@"Alfredo"];
//    [self.window center];
    [self.window makeKeyAndOrderFront:nil];

    [NSApp activateIgnoringOtherApps:YES];
    [NSApp stop:nil];
  }
}

- (void)applicationWillTerminate:(NSNotification *)notification {
  delete _metalRenderer;
  delete _metalTextureLoader;
}

@end
