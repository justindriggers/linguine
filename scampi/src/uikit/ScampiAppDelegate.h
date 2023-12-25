#import <UIKit/UIKit.h>

#include <memory>

#import "../platform/IosTimeManager.h"

@interface ScampiAppDelegate : UIResponder <UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;
@property (nonatomic) std::shared_ptr<linguine::scampi::IosTimeManager> timeManager;
@end
