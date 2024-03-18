#import "AlfredoApplicationDelegate.h"

#import <AudioEngineAudioManager.h>

#import "platform/MacAudioEngineFileLoader.h"
#import "platform/MacInputManager.h"
#import "platform/MacLeaderboardManager.h"
#import "platform/MacLifecycleManager.h"
#import "platform/MacLogger.h"
#import "platform/MacMetalTextureLoader.h"
#import "platform/MacSaveManager.h"
#import "platform/MacTimeManager.h"
#import "platform/MacUrlHander.h"

using namespace linguine;
using namespace linguine::alfredo;

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

    NSString *appName = [[NSRunningApplication currentApplication] localizedName];
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
    self.view = [[MTKView alloc] initWithFrame:frame
                                   device:self.device];
    [self.view setPaused:YES];
    [self.view setEnableSetNeedsDisplay:NO];
    ((CAMetalLayer*)[self.view layer]).displaySyncEnabled = NO;

    self.lifecycleManager = std::make_shared<MacLifecycleManager>();

    auto mtkTextureLoader = [[MTKTextureLoader alloc] initWithDevice:self.device];
    auto metalTextureLoader = std::make_unique<MacMetalTextureLoader>(mtkTextureLoader);
    auto metalRenderBackend = render::MetalRenderBackend::create(*(__bridge MTK::View*)self.view, std::move(metalTextureLoader));

    auto audioFileLoader = std::make_unique<MacAudioEngineFileLoader>();

    auto logger = std::make_shared<MacLogger>();
    auto audioManager = std::make_shared<audio::AudioEngineAudioManager>(std::move(audioFileLoader));
    auto inputManager = std::make_shared<MacInputManager>();
    auto leaderboardManager = std::make_shared<MacLeaderboardManager>();
    auto renderer = std::make_shared<Renderer>(std::move(metalRenderBackend));
    auto saveManager = std::make_shared<MacSaveManager>();
    auto timeManager = std::make_shared<MacTimeManager>();
    auto urlHandler = std::make_shared<MacUrlHandler>();
    auto engine = std::make_unique<Engine>(logger, audioManager, inputManager, leaderboardManager, self.lifecycleManager, renderer, saveManager, timeManager, urlHandler);

    _viewDelegate = [[AlfredoViewDelegate alloc] initWithEngine:std::move(engine)];

    [_viewDelegate mtkView:self.view drawableSizeWillChange:self.view.bounds.size];
    [self.view setDelegate:_viewDelegate];

    [self.window setContentView:self.view];
    [self.window setTitle:@"Alfredo"];
    [self.window makeKeyAndOrderFront:nil];

    [NSApp activateIgnoringOtherApps:YES];
    [NSApp stop:nil];
  }
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
  return YES;
}

@end
