#pragma once

#include <OpenGLFileLoader.h>

#include <android/asset_manager.h>

namespace linguine::carbonara {

class AndroidOpenGLFileLoader : public render::OpenGLFileLoader {
  public:
    explicit AndroidOpenGLFileLoader(AAssetManager& assetManager)
        : _assetManager(assetManager) {}

    ~AndroidOpenGLFileLoader() override = default;

    [[nodiscard]] ImageFile getImage() const override;

  private:
    AAssetManager& _assetManager;
};

}  // namespace linguine::carbonara
