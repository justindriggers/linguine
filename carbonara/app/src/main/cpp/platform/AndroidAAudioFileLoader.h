#pragma once

#include <AAudioFileLoader.h>

#include <android/asset_manager.h>

namespace linguine::carbonara {

class AndroidAAudioFileLoader : public audio::AAudioFileLoader {
  public:
    explicit AndroidAAudioFileLoader(AAssetManager& assetManager)
        : _assetManager(assetManager) {}

    [[nodiscard]] std::vector<std::byte> getAudioDataForEffect(EffectType effectType) const override;

    [[nodiscard]] std::vector<std::byte> getAudioDataForSong(SongType songType) const override;

  private:
    AAssetManager& _assetManager;

    [[nodiscard]] std::vector<std::byte> loadFile(std::string filename) const;
};

}  // namespace linguine::carbonara
