#import <Cocoa/Cocoa.h>

#import "MacLifecycleManager.h"

bool linguine::alfredo::MacLifecycleManager::isRunning() const {
  @autoreleasepool {
    return [[NSApp windows] count] > 0;
  }
}
