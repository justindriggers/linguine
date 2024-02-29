#import <AudioEngineAudioManager.h>

#import "appkit/AlfredoApplicationDelegate.h"

using namespace linguine;

int main(int argc, const char *argv[]) {
  @autoreleasepool {
    NSApplication *app = NSApplication.sharedApplication;

    AlfredoApplicationDelegate *appDelegate = [[AlfredoApplicationDelegate alloc] init];
    app.delegate = appDelegate;

    [app run];

    return 0;
  }
}
