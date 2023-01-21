#import <Cocoa/Cocoa.h>
#import <memory>

#import <Engine.h>

#import "appkit/AlfredoApplicationDelegate.h"
#import "platform/MacInputManager.h"
#import "platform/MacLifecycleManager.h"
#import "platform/MacTimeManager.h"

using namespace linguine;
using namespace linguine::alfredo;

int main(int argc, const char *argv[]) {
  @autoreleasepool {
    NSApplication *app = NSApplication.sharedApplication;

    AlfredoApplicationDelegate *appDelegate = [[AlfredoApplicationDelegate alloc] init];
    app.delegate = appDelegate;

    [app run];

    auto inputManager = std::shared_ptr<InputManager>(new MacInputManager());
    auto lifecycleManager = std::shared_ptr<LifecycleManager>(new MacLifecycleManager());
    auto timeManager = std::shared_ptr<TimeManager>(new MacTimeManager());
    auto engine = Engine(inputManager, lifecycleManager, timeManager);

    engine.run();

    return 0;
  }
}