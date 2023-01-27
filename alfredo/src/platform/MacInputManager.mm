#import "MacInputManager.h"

#import <Cocoa/Cocoa.h>

void linguine::alfredo::MacInputManager::pollEvents() {
  @autoreleasepool {
    NSApplication *app = NSApplication.sharedApplication;

    NSEvent *event;
    while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                     untilDate:nil
                                        inMode:NSDefaultRunLoopMode
                                       dequeue:TRUE]) != nil) {
      [NSApp sendEvent:event];
    }
  }
}
