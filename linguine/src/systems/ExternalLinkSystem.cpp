#include "ExternalLinkSystem.h"

#include "components/ExternalLink.h"
#include "components/Tapped.h"

namespace linguine {

void ExternalLinkSystem::update(float deltaTime) {
  findEntities<ExternalLink, Tapped>()->each([this](const Entity& entity) {
    auto externalLink = entity.get<ExternalLink>();
    _urlHandler.open(externalLink->url);
  });
}

}  // namespace linguine
