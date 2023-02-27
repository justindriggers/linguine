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

    auto logger = std::make_shared<MacLogger>();
    auto inputManager = std::make_shared<MacInputManager>();
    auto lifecycleManager = std::make_shared<MacLifecycleManager>();
    auto renderer = std::shared_ptr<Renderer>(appDelegate.metalRenderer);
    auto timeManager = std::make_shared<IosTimeManager>();
    auto engine = Engine(logger, inputManager, lifecycleManager, renderer, timeManager);

    engine.run();

    return 0;
  }
}