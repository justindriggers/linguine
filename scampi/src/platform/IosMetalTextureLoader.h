#pragma once

#include <MetalTextureLoader.h>

#include <string>
#include <unordered_map>

#include <MetalKit/MetalKit.h>

namespace linguine::scampi {

class IosMetalTextureLoader : public render::MetalTextureLoader {
  public:
    explicit IosMetalTextureLoader(MTKTextureLoader* mtkTextureLoader)
        : _mtkTextureLoader(mtkTextureLoader) {}

    ~IosMetalTextureLoader() override;

    MTL::Texture* getTexture() override;

  private:
    MTKTextureLoader* _mtkTextureLoader;

    std::unordered_map<std::string, MTL::Texture*> _textures;
};

}  // namespace linguine::scampi
