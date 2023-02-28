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
  std::shared_ptr<linguine::scampi::IosInputManager> _inputManager;
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

  _inputManager = std::make_shared<linguine::scampi::IosInputManager>();

  auto logger = std::make_shared<linguine::scampi::NSLogger>();
  auto lifecycleManager = std::make_shared<linguine::scampi::IosLifecycleManager>();
  auto renderer = std::shared_ptr<linguine::Renderer>(linguine::render::MetalRenderer::create(*(__bridge MTK::View*)_view, true));
  auto timeManager = std::make_shared<linguine::scampi::IosTimeManager>();

  auto engine = std::make_shared<linguine::Engine>(logger, _inputManager, lifecycleManager, renderer, timeManager);

  _viewDelegate = [[ScampiViewDelegate alloc] initWithEngine:engine
                                                    renderer:renderer];

  [_viewDelegate mtkView:_view drawableSizeWillChange:_view.bounds.size];
  [_view setDelegate:_viewDelegate];
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches
           withEvent:(nullable UIEvent *)event {
  for (UITouch *touch in touches) {
    CGPoint point = [touch locationInView:touch.view];
    linguine::scampi::IosInputManager::TouchEvent touchEvent = {
        .x = static_cast<float>(point.x / _view.bounds.size.width),
        .y = static_cast<float>(1.0f - point.y / _view.bounds.size.height),
        .isActive = true
    };

    _inputManager->enqueue((uintptr_t)touch, touchEvent);
  }
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches
           withEvent:(nullable UIEvent *)event {
  for (UITouch *touch in touches) {
    CGPoint point = [touch locationInView:touch.view];
    linguine::scampi::IosInputManager::TouchEvent touchEvent = {
        .x = static_cast<float>(point.x / _view.bounds.size.width),
        .y = static_cast<float>(1.0f - point.y / _view.bounds.size.height),
        .isActive = true
    };

    _inputManager->enqueue((uintptr_t)touch, touchEvent);
  }
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches
           withEvent:(nullable UIEvent *)event {
  for (UITouch *touch in touches) {
    CGPoint point = [touch locationInView:touch.view];
    linguine::scampi::IosInputManager::TouchEvent touchEvent = {
        .x = static_cast<float>(point.x / _view.bounds.size.width),
        .y = static_cast<float>(1.0f - point.y / _view.bounds.size.height),
        .isActive = false
    };

    _inputManager->enqueue((uintptr_t)touch, touchEvent);
  }
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches
               withEvent:(nullable UIEvent *)event {
  for (UITouch *touch in touches) {
    CGPoint point = [touch locationInView:touch.view];
    linguine::scampi::IosInputManager::TouchEvent touchEvent = {
        .x = static_cast<float>(point.x / _view.bounds.size.width),
        .y = static_cast<float>(1.0f - point.y / _view.bounds.size.height),
        .isActive = false
    };

    _inputManager->enqueue((uintptr_t)touch, touchEvent);
  }
}

@end
