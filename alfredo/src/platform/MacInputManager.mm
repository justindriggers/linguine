#import "MacInputManager.h"

#import <Cocoa/Cocoa.h>

#import <vector>

namespace linguine::alfredo {

void MacInputManager::pollEvents() {
  @autoreleasepool {
    NSApplication *app = NSApplication.sharedApplication;
    auto window = [app mainWindow];
    auto frameSize = window.contentView.frame.size;

    auto toRemove = std::vector<uint64_t>();

    for (auto& touch : _active) {
      switch (touch.second.state) {
        case InputManager::Down:
          touch.second.state = Hold;
          break;
        case TouchState::Up:
          toRemove.push_back(touch.first);
          break;
        default:
          break;
      }
    }

    for (const auto id : toRemove) {
      _active.erase(id);
    }

    NSEvent *event;
    while ((event = [app nextEventMatchingMask:NSEventMaskAny
                                     untilDate:nil
                                        inMode:NSDefaultRunLoopMode
                                       dequeue:TRUE]) != nil) {
      switch (event.type) {
        case NSEventTypeLeftMouseDown: {
          auto mouseLocation = event.locationInWindow;

          if (mouseLocation.x < 0.0f || mouseLocation.x > frameSize.width
              || mouseLocation.y < 0.0f || mouseLocation.y > frameSize.height) {
            break;
          }

          auto touch = Touch {
              .x = static_cast<float>(mouseLocation.x / frameSize.width),
              .y = static_cast<float>(mouseLocation.y / frameSize.height),
              .state = TouchState::Down
          };

          _active.insert({0, touch});
          break;
        }
        case NSEventTypeLeftMouseDragged: {
          auto mouseLocation = event.locationInWindow;

          if (mouseLocation.x < 0.0f || mouseLocation.x > frameSize.width
              || mouseLocation.y < 0.0f || mouseLocation.y > frameSize.height) {
            break;
          }

          auto& touch = _active[0];
          touch.x = static_cast<float>(mouseLocation.x / frameSize.width);
          touch.y = static_cast<float>(mouseLocation.y / frameSize.height);
          break;
        }
        case NSEventTypeLeftMouseUp: {
          auto mouseLocation = event.locationInWindow;
          auto& touch = _active[0];
          touch.state = TouchState::Up;

          if (mouseLocation.x < 0.0f || mouseLocation.x > frameSize.width
              || mouseLocation.y < 0.0f || mouseLocation.y > frameSize.height) {
            break;
          }

          touch.x = static_cast<float>(mouseLocation.x / frameSize.width);
          touch.y = static_cast<float>(mouseLocation.y / frameSize.height);
          break;
        }
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

float MacInputManager::getSensitivity() const {
  return 1350.0f;
}

}  // namespace linguine::alfredo
