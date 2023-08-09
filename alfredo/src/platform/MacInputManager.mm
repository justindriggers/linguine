#import "MacInputManager.h"

#import <Carbon/Carbon.h>
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
        case NSEventTypeMouseMoved: {
          auto mouseLocation = [window convertPointFromScreen:[NSEvent mouseLocation]];

          if (mouseLocation.x < 0.0f || mouseLocation.x > frameSize.width
              || mouseLocation.y < 0.0f || mouseLocation.y > frameSize.height) {
            break;
          }

          _cursorLocation.x = static_cast<float>(mouseLocation.x / frameSize.width);
          _cursorLocation.y = static_cast<float>(mouseLocation.y / frameSize.height);
          break;
        }
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
        case NSEventTypeKeyDown: {
          switch (event.keyCode) {
          case kVK_ANSI_Q:
            _keyStates[Key::Q] = true;

            if (event.modifierFlags & NSEventModifierFlagCommand) {
              [app sendEvent:event];
            }
            break;
          case kVK_ANSI_W:
            _keyStates[Key::W] = true;
            break;
          case kVK_ANSI_E:
            _keyStates[Key::E] = true;
            break;
          case kVK_ANSI_R:
            _keyStates[Key::R] = true;
            break;
          case kVK_ANSI_A:
            _keyStates[Key::A] = true;
            break;
          case kVK_ANSI_S:
            _keyStates[Key::S] = true;
            break;
          case kVK_ANSI_D:
            _keyStates[Key::D] = true;
            break;
          case kVK_ANSI_F:
            _keyStates[Key::F] = true;
            break;
          case kVK_ANSI_C:
            _keyStates[Key::C] = true;
            break;
          default:
            [app sendEvent:event];
            break;
          }
          break;
        }
        case NSEventTypeKeyUp: {
          switch (event.keyCode) {
          case kVK_ANSI_Q:
            _keyStates[Key::Q] = false;

            if (event.modifierFlags & NSEventModifierFlagCommand) {
              [app sendEvent:event];
            }
            break;
          case kVK_ANSI_W:
            _keyStates[Key::W] = false;
            break;
          case kVK_ANSI_E:
            _keyStates[Key::E] = false;
            break;
          case kVK_ANSI_R:
            _keyStates[Key::R] = false;
            break;
          case kVK_ANSI_A:
            _keyStates[Key::A] = false;
            break;
          case kVK_ANSI_S:
            _keyStates[Key::S] = false;
            break;
          case kVK_ANSI_D:
            _keyStates[Key::D] = false;
            break;
          case kVK_ANSI_F:
            _keyStates[Key::F] = false;
            break;
          case kVK_ANSI_C:
            _keyStates[Key::C] = false;
            break;
          default:
            [app sendEvent:event];
            break;
          }
          break;
        }
        default:
          break;
      }
    }
  }
}

const std::unordered_map<uint64_t, InputManager::Touch>& MacInputManager::getTouches() const {
  return _active;
}

float MacInputManager::getSensitivity() const {
  return 1350.0f;
}

bool MacInputManager::isKeyPressed(Key key) const {
  return _keyStates[key];
}

InputManager::CursorLocation MacInputManager::getCursorLocation() const {
  return _cursorLocation;
}

}  // namespace linguine::alfredo
