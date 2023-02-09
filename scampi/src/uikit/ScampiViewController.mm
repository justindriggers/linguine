#import "ScampiViewController.h"

#import <MetalRenderer.h>

#import "../metalkit/ScampiViewDelegate.h"
#import "../platform/IosInputManager.h"
#import "../platform/IosLifecycleManager.h"
#import "../platform/IosTimeManager.h"
#import "../platform/NSLogger.h"

@implementation ScampiViewController {
  MTKView *_view;
  ScampiViewDelegate *_viewDelegate;
}

- (void)viewDidLoad
{
  [super viewDidLoad];

  _view = (MTKView *)self.view;

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

  auto logger = std::shared_ptr<linguine::Logger>(new linguine::scampi::NSLogger());
  auto inputManager = std::shared_ptr<linguine::InputManager>(new linguine::scampi::IosInputManager());
  auto lifecycleManager = std::shared_ptr<linguine::LifecycleManager>(new linguine::scampi::IosLifecycleManager());
  auto renderer = std::shared_ptr<linguine::Renderer>(linguine::render::MetalRenderer::create(*(__bridge MTK::View*)_view, true));
  auto timeManager = std::shared_ptr<linguine::TimeManager>(new linguine::scampi::IosTimeManager());

  auto engine = new linguine::Engine(logger, inputManager, lifecycleManager, renderer, timeManager);

  _viewDelegate = [[ScampiViewDelegate alloc] initWithEngine:engine];

  [_viewDelegate mtkView:_view drawableSizeWillChange:_view.bounds.size];
  [_view setDelegate:_viewDelegate];
}

@end
