#include "AndroidSaveManager.h"

namespace linguine::carbonara {

void AndroidSaveManager::load()  {
  _points = _sharedPreferences.getInt("points", 0);
  _lives = static_cast<uint8_t>(_sharedPreferences.getInt("lives", 0));
  _isMusicEnabled = _sharedPreferences.getBoolean("music", true);
  _isSoundEffectsEnabled = _sharedPreferences.getBoolean("sfx", true);
  _isScreenShakeEnabled = _sharedPreferences.getBoolean("shake", true);
  _handedness = _sharedPreferences.getInt("handedness", 0) == 0 ? Handedness::Right : Handedness::Left;
  _isNewPlayer = _sharedPreferences.getBoolean("new", true);
}

void AndroidSaveManager::save() {
  _sharedPreferences.edit()
      .putInt("points", _points)
      .putInt("lives", _lives)
      .putBoolean("music", _isMusicEnabled)
      .putBoolean("sfx", _isSoundEffectsEnabled)
      .putBoolean("shake", _isScreenShakeEnabled)
      .putInt("handedness", _handedness == SaveManager::Handedness::Right ? 0 : 1)
      .putBoolean("new", _isNewPlayer)
      .apply();
}

}  // namespace linguine::carbonara
