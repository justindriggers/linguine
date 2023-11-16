#include "WalletSystem.h"

#include "components/Text.h"
#include "components/shop/Wallet.h"

namespace linguine {

void WalletSystem::update(float deltaTime) {
  findEntities<Wallet, Text>()->each([](const Entity& entity) {
    auto wallet = entity.get<Wallet>();
    auto text = entity.get<Text>();

    text->feature->text = std::to_string(wallet->points);
  });
}

}  // namespace linguine
