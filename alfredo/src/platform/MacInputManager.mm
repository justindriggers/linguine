#import "MacInputManager.h"

#import <Cocoa/Cocoa.h>

namespace linguine::alfredo {

void MacInputManager::pollEvents() {
  @autoreleasepool {
    NSApplication *app = NSApplication.sharedApplication;

    NSEvent *event;
    while ((event = [app nextEventMatchingMask:NSEventMaskAny
                                     untilDate:nil
                                        inMode:NSDefaultRunLoopMode
                                       dequeue:TRUE]) != nil) {
      switch (event.type) {
      case NSEventTypeLeftMouseDown: {
        auto mouseLocation = event.locationInWindow;
        auto touch = Touch {
            .x = static_cast<float>(mouseLocation.x),
            .y = static_cast<float>(mouseLocation.y)
        };
        _active.insert({0, touch});
        break;
      }
      case NSEventTypeLeftMouseDragged: {
        auto window = [app mainWindow];
        auto frameSize = window.frame.size;
        auto mouseLocation = event.locationInWindow;

        if (mouseLocation.x < 0.0f || mouseLocation.x > frameSize.width
            || mouseLocation.y < 0.0f || mouseLocation.y > frameSize.height) {
          break;
        }

        auto& touch = _active[0];
        touch.x = static_cast<float>(mouseLocation.x);
        touch.y = static_cast<float>(mouseLocation.y);
        break;
      }
      case NSEventTypeLeftMouseUp:
        _active.erase(0);
        break;
      default:
        break;
      }

      [app sendEvent:event];
    }
  }
}

const std::unordered_map<uint64_t, InputManager::Touch>& MacInputManager::getTouches() const {
  return _active;
}

}  // namespace linguine::alfredo
