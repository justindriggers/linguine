#import "MacLifecycleManager.h"

#import <Cocoa/Cocoa.h>

bool linguine::alfredo::MacLifecycleManager::isRunning() const {
  @autoreleasepool {
    return [[NSApp windows] count] > 0;
  }
}
