#import "ScampiViewController.h"

#import <AVFoundation/AVAudioSession.h>
#import <GameKit/GKLocalPlayer.h>

#import <AudioEngineAudioManager.h>
#import <MetalRenderBackend.h>

#import "ScampiAppDelegate.h"
#import "metalkit/ScampiViewDelegate.h"
#import "platform/IosAudioEngineFileLoader.h"
#import "platform/IosInputManager.h"
#import "platform/IosLeaderboardManager.h"
#import "platform/IosLifecycleManager.h"
#import "platform/IosMetalTextureLoader.h"
#import "platform/IosSaveManager.h"
#import "platform/IosTimeManager.h"
#import "platform/NSLogger.h"

using namespace linguine;

@implementation ScampiViewController {
  MTKView *_view;
  ScampiViewDelegate *_viewDelegate;
  std::shared_ptr<linguine::scampi::IosInputManager> _inputManager;
}

- (void)viewDidLoad
{
  [super viewDidLoad];

  [self setNeedsUpdateOfHomeIndicatorAutoHidden];

  _view = (MTKView *)self.view;
  _view.multipleTouchEnabled = true;
  _view.preferredFramesPerSecond = 120;

  _view.device = MTLCreateSystemDefaultDevice();
  _view.backgroundColor = UIColor.blackColor;

  if (!_view.device) {
    NSLog(@"Metal is not supported on this device");
    self.view = [[UIView alloc] initWithFrame:self.view.frame];
    return;
  }

  [_view setPaused:NO];
  [_view setEnableSetNeedsDisplay:NO];
  [_view setPreferredFramesPerSecond:120];

  auto audioSession = [AVAudioSession sharedInstance];

  NSError* error;
  if (![audioSession setCategory:AVAudioSessionCategoryAmbient
                           error:&error]) {
    NSLog(@"%@", [error localizedDescription]);
    return;
  }

  if (![audioSession setActive:true
                   withOptions:AVAudioSessionSetActiveOptionNotifyOthersOnDeactivation
                         error:&error]) {
    NSLog(@"%@", [error localizedDescription]);
    return;
  }

  _inputManager = std::make_shared<linguine::scampi::IosInputManager>();

  UISwipeGestureRecognizer *swipeLeft = [[UISwipeGestureRecognizer alloc] initWithTarget:self
                                                                                  action:@selector(leftSwipe:)];
  [swipeLeft setDirection: UISwipeGestureRecognizerDirectionLeft];
  [_view addGestureRecognizer:swipeLeft];

  UISwipeGestureRecognizer *swipeRight = [[UISwipeGestureRecognizer alloc] initWithTarget:self
                                                                                   action:@selector(rightSwipe:)];
  [swipeRight setDirection: UISwipeGestureRecognizerDirectionRight];
  [_view addGestureRecognizer:swipeRight];

  auto logger = std::make_shared<linguine::scampi::NSLogger>();
  auto leaderboardManager = std::make_shared<linguine::scampi::IosLeaderboardManager>();
  auto lifecycleManager = std::make_shared<linguine::scampi::IosLifecycleManager>();
  auto timeManager = std::make_shared<linguine::scampi::IosTimeManager>();
  auto audioFileLoader = std::make_unique<linguine::scampi::IosAudioEngineFileLoader>();
  auto audioManager = std::make_shared<linguine::audio::AudioEngineAudioManager>(std::move(audioFileLoader));

  auto mtkTextureLoader = [[MTKTextureLoader alloc] initWithDevice:_view.device];
  auto textureLoader = std::make_unique<linguine::scampi::IosMetalTextureLoader>(mtkTextureLoader);
  auto metalRenderBackend = render::MetalRenderBackend::create(*(__bridge MTK::View*)_view, std::move(textureLoader));
  auto renderer = std::make_shared<Renderer>(std::move(metalRenderBackend));

  auto insets = UIApplication.sharedApplication.windows.firstObject.safeAreaInsets;
  renderer->setInsets(static_cast<uint16_t>(insets.left), static_cast<uint16_t>(insets.right),
      static_cast<uint16_t>(insets.top), static_cast<uint16_t>(insets.bottom));

  auto saveManager = std::make_shared<linguine::scampi::IosSaveManager>();

  auto appDelegate = (ScampiAppDelegate *)[[UIApplication sharedApplication] delegate];

  auto engine = std::make_shared<linguine::Engine>(logger, audioManager, _inputManager, leaderboardManager, lifecycleManager, renderer, saveManager, timeManager);
  appDelegate.engine = engine;

  _viewDelegate = [[ScampiViewDelegate alloc] initWithEngine:engine];

  [_viewDelegate mtkView:_view drawableSizeWillChange:_view.bounds.size];
  [_view setDelegate:_viewDelegate];

  GKLocalPlayer.localPlayer.authenticateHandler = ^(UIViewController *viewController, NSError *authError) {
    if (viewController) {
      [self presentViewController:viewController animated:YES completion:nil];
      return;
    }

    if (authError) {
      NSLog(@"Game Center Error: %@", authError.localizedDescription);
      return;
    }
  };
}

- (BOOL)prefersStatusBarHidden {
  return YES;
}

-(BOOL)prefersHomeIndicatorAutoHidden {
  return YES;
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches
           withEvent:(nullable UIEvent *)event {
  for (UITouch *touch in touches) {
    CGPoint point = [touch locationInView:touch.view];
    _inputManager->onTouchBegan(
        (uintptr_t)touch,
        static_cast<float>(point.x / _view.bounds.size.width),
        static_cast<float>(1.0f - point.y / _view.bounds.size.height)
    );
  }
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches
           withEvent:(nullable UIEvent *)event {
  for (UITouch *touch in touches) {
    CGPoint point = [touch locationInView:touch.view];
    _inputManager->onTouchMoved(
        (uintptr_t)touch,
        static_cast<float>(point.x / _view.bounds.size.width),
        static_cast<float>(1.0f - point.y / _view.bounds.size.height)
    );
  }
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches
           withEvent:(nullable UIEvent *)event {
  for (UITouch *touch in touches) {
    CGPoint point = [touch locationInView:touch.view];
    _inputManager->onTouchEnded(
        (uintptr_t)touch,
        static_cast<float>(point.x / _view.bounds.size.width),
        static_cast<float>(1.0f - point.y / _view.bounds.size.height)
    );
  }
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches
               withEvent:(nullable UIEvent *)event {
  for (UITouch *touch in touches) {
    CGPoint point = [touch locationInView:touch.view];
    _inputManager->onTouchCanceled(
        (uintptr_t)touch,
        static_cast<float>(point.x / _view.bounds.size.width),
        static_cast<float>(1.0f - point.y / _view.bounds.size.height)
    );
  }
}

- (void)leftSwipe:(UISwipeGestureRecognizer *)gesture {
  _inputManager->onLeftSwipe();
}

-(void)rightSwipe:(UISwipeGestureRecognizer *)gesture {
  _inputManager->onRightSwipe();
}

@end
