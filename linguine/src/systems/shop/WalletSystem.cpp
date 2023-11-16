#include "WalletSystem.h"

#include "components/Text.h"
#include "components/shop/Wallet.h"

namespace linguine {

void WalletSystem::update(float deltaTime) {
  findEntities<Wallet, Text>()->each([this](const Entity& entity) {
    auto text = entity.get<Text>();

    text->feature->text = std::to_string(_saveManager.getPoints());
  });
}

}  // namespace linguine
