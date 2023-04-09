#import <memory>

#import <Cocoa/Cocoa.h>

#import <AudioEngineAudioManager.h>
#import <Engine.h>

#import "appkit/AlfredoApplicationDelegate.h"
#import "platform/MacAudioEngineFileLoader.h"
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

    auto audioFileLoader = std::make_unique<MacAudioEngineFileLoader>();

    auto logger = std::make_shared<MacLogger>();
    auto audioManager = std::make_shared<audio::AudioEngineAudioManager>(std::move(audioFileLoader));
    auto inputManager = std::make_shared<MacInputManager>();
    auto lifecycleManager = std::make_shared<MacLifecycleManager>();
    auto renderer = std::shared_ptr<Renderer>(appDelegate.metalRenderer);
    auto timeManager = std::make_shared<MacTimeManager>();
    auto engine = Engine(logger, audioManager, inputManager, lifecycleManager, renderer, timeManager);

    engine.run();

    return 0;
  }
}