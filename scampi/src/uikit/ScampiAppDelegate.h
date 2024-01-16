#import <UIKit/UIKit.h>

#include <memory>

#import <Engine.h>

@interface ScampiAppDelegate : UIResponder <UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;
@property (nonatomic) std::shared_ptr<linguine::Engine> engine;
@end
