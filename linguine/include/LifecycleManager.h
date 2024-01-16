#pragma once

namespace linguine {

class LifecycleManager {
  public:
    virtual ~LifecycleManager() = default;

    [[nodiscard]] virtual bool isRunning() const = 0;

    [[nodiscard]] inline bool isPaused() const {
      return _isPaused;
    }

    inline void pause() {
      _isPaused = true;
    }

    inline void resume() {
      _isPaused = false;
    }

  private:
    bool _isPaused = false;
};

}  // namespace linguine
