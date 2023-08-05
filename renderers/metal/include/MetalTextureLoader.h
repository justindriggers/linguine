#pragma once

namespace MTL {

  class Texture;

}  // namespace MTL

namespace linguine::render {

class MetalTextureLoader {
  public:
    virtual ~MetalTextureLoader() = default;

    virtual MTL::Texture* getTexture() = 0;
};

}  // namespace linguine::render
