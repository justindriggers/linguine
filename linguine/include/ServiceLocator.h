#pragma one

#include <stdexcept>

#include "entity/EntityManagerFactory.h"
#include "renderer/Renderer.h"
#include "InputManager.h"
#include "LifecycleManager.h"
#include "Logger.h"
#include "TimeManager.h"

namespace linguine {

class ServiceLocator {
  public:
    template<typename T>
    inline T& get() {
      static_assert(undefined_service<T>::value, "Service is undefined");
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
    inline TimeManager& get<TimeManager>() {
      return getTimeManager();
    }

  private:
    template<typename>
    struct undefined_service : std::false_type {};

    virtual EntityManagerFactory& getEntityManagerFactory() = 0;

    virtual InputManager& getInputManager() = 0;

    virtual LifecycleManager& getLifecycleManager() = 0;

    virtual Logger& getLogger() = 0;

    virtual Renderer& getRenderer() = 0;

    virtual TimeManager& getTimeManager() = 0;
};

}  // namespace linguine
