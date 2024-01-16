#pragma once

#include <unordered_map>

#import <AVFoundation/AVAudioFile.h>

#include <audio/EffectType.h>
#include <audio/SongType.h>

namespace linguine::audio {

class AudioEngineFileLoader {
  public:
    virtual ~AudioEngineFileLoader() = default;

    AVAudioFile* getAudioFileForEffect(EffectType effectType);

    AVAudioFile* getAudioFileForSong(SongType songType);

    [[nodiscard]] int64_t getSongLoopPoint(SongType songType) const {
      return _songLoopPoints.at(songType);
    }

    virtual NSURL* getUrlForEffect(EffectType effectType) = 0;

    virtual NSURL* getUrlForSong(SongType songType) = 0;

  private:
    std::unordered_map<EffectType, AVAudioFile*> _loadedEffects;
    std::unordered_map<SongType, AVAudioFile*> _loadedSongs;

    const std::unordered_map<SongType, int64_t> _songLoopPoints = {
        { SongType::Title, 1058400 },
        { SongType::Theme, 2646000 },
        { SongType::GameOver, 529200 }
    };
};

}  // namespace linguine::audio
