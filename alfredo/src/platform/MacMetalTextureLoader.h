#pragma once

#include <MetalTextureLoader.h>

#include <string>
#include <unordered_map>

#include <MetalKit/MetalKit.h>

namespace linguine::alfredo {

class MacMetalTextureLoader : public render::MetalTextureLoader {
  public:
    explicit MacMetalTextureLoader(MTKTextureLoader* mtkTextureLoader)
        : _mtkTextureLoader(mtkTextureLoader) {}

    ~MacMetalTextureLoader() override;

    MTL::Texture* getTexture() override;

  private:
    MTKTextureLoader* _mtkTextureLoader;

    std::unordered_map<std::string, MTL::Texture*> _textures;
};

}  // namespace linguine::alfredo
