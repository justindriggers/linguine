#import "AlfredoApplicationDelegate.h"

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
    self.window = [[NSWindow alloc]
        initWithContentRect:CGRectMake(100.0, 100.0, 512.0, 512.0)
                  styleMask:NSWindowStyleMaskClosable | NSWindowStyleMaskTitled
                    backing:NSBackingStoreBuffered
                      defer:NO];
    self.window.title = @"Alfredo";

    [NSApp activateIgnoringOtherApps:YES];
    [self.window center];
    [self.window makeKeyAndOrderFront:nil];

    [NSApp stop:nil];
  }
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app {
  return YES;
}

@end
