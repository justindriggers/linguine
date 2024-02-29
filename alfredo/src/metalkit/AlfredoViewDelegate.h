#import <MetalKit/MetalKit.h>

#import <Engine.h>

using namespace linguine;

@interface AlfredoViewDelegate : NSObject <MTKViewDelegate> {
  @private
    std::unique_ptr<Engine> _engine;
}

- (instancetype)initWithEngine:(std::unique_ptr<Engine>)engine;

@end