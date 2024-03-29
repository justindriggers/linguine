#pragma once

#include <stdexcept>

#include "LeaderboardManager.h"
#include "LifecycleManager.h"
#include "Logger.h"
#include "SaveManager.h"
#include "SceneManager.h"
#include "TimeManager.h"
#include "UrlHandler.h"
#include "audio/AudioManager.h"
#include "entity/EntityManagerFactory.h"
#include "input/InputManager.h"
#include "renderer/Renderer.h"

namespace linguine {

class ServiceLocator {
  public:
    template<typename T>
    inline T& get() {
      static_assert(undefined_service<T>::value, "Service is undefined");
    }

    template<>
    inline AudioManager& get<AudioManager>() {
      return getAudioManager();
    }

    template<>
    inline EntityManagerFactory& get<EntityManagerFactory>() {
      return getEntityManagerFactory();
    }

    template<>
    inline InputManager& get<InputManager>() {
      return getInputManager();
    }

    template<>
    inline LeaderboardManager& get<LeaderboardManager>() {
      return getLeaderboardManager();
    }

    template<>
    inline LifecycleManager& get<LifecycleManager>() {
      return getLifecycleManager();
    }

    template<>
    inline Logger& get<Logger>() {
      return getLogger();
    }

    template<>
    inline Renderer& get<Renderer>() {
      return getRenderer();
    }

    template<>
    inline SaveManager& get<SaveManager>() {
      return getSaveManager();
    }

    template<>
    inline SceneManager& get<SceneManager>() {
      return getSceneManager();
    }

    template<>
    inline ServiceLocator& get<ServiceLocator>() {
      return *this;
    }

    template<>
    inline TimeManager& get<TimeManager>() {
      return getTimeManager();
    }

    template<>
    inline UrlHandler& get<UrlHandler>() {
      return getUrlHandler();
    }

  private:
    template<typename>
    struct undefined_service : std::false_type {};

    virtual AudioManager& getAudioManager() = 0;

    virtual EntityManagerFactory& getEntityManagerFactory() = 0;

    virtual InputManager& getInputManager() = 0;

    virtual LeaderboardManager& getLeaderboardManager() = 0;

    virtual LifecycleManager& getLifecycleManager() = 0;

    virtual Logger& getLogger() = 0;

    virtual Renderer& getRenderer() = 0;

    virtual SaveManager& getSaveManager() = 0;

    virtual SceneManager& getSceneManager() = 0;

    virtual TimeManager& getTimeManager() = 0;

    virtual UrlHandler& getUrlHandler() = 0;
};

}  // namespace linguine
