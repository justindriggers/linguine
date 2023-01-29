#import <memory>

#import <Cocoa/Cocoa.h>

#import <Engine.h>

#import "appkit/AlfredoApplicationDelegate.h"
#import "platform/MacInputManager.h"
#import "platform/MacLifecycleManager.h"
#import "platform/MacLogger.h"
#import "platform/MacTimeManager.h"

using namespace linguine;
using namespace linguine::alfredo;

int main(int argc, const char *argv[]) {
  @autoreleasepool {
    NSApplication *app = NSApplication.sharedApplication;

    AlfredoApplicationDelegate *appDelegate = [[AlfredoApplicationDelegate alloc] init];
    app.delegate = appDelegate;

    [app run];

    auto logger = std::shared_ptr<Logger>(new MacLogger());
    auto inputManager = std::shared_ptr<InputManager>(new MacInputManager());
    auto lifecycleManager = std::shared_ptr<LifecycleManager>(new MacLifecycleManager());
    auto renderer = std::shared_ptr<Renderer>(appDelegate.mtkRenderer);
    auto timeManager = std::shared_ptr<TimeManager>(new IosTimeManager());
    auto engine = Engine(logger, inputManager, lifecycleManager, renderer, timeManager);

    engine.run();

    return 0;
  }
}