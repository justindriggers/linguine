#pragma once

#include "System.h"

#include "UrlHandler.h"

namespace linguine {

class ExternalLinkSystem : public System {
  public:
    ExternalLinkSystem(EntityManager& entityManager,
                       UrlHandler& urlHandler)
        : System(entityManager), _urlHandler(urlHandler) {}

    void update(float deltaTime) override;

    void fixedUpdate(float fixedDeltaTime) override {}

  private:
    UrlHandler& _urlHandler;
};

}  // namespace linguine
