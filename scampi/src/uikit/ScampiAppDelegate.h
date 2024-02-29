#import <UIKit/UIKit.h>

#import <Engine.h>

using namespace linguine;

@interface ScampiAppDelegate : UIResponder <UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;
@property (nonatomic) std::shared_ptr<Engine> engine;
@end
