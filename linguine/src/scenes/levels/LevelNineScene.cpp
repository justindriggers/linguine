#include "LevelNineScene.h"

#include "components/Health.h"

namespace linguine {

void LevelNineScene::init() {
  RunnerScene::init();

  auto randomHealth = std::uniform_real_distribution(0.25f, 0.85f);

  getEntityManager().find<Health>()->each([this, &randomHealth](const Entity& entity) {
    auto health = entity.get<Health>();
    health->current = static_cast<int32_t>(glm::round(randomHealth(_random) * static_cast<float>(health->max)));
  });
}

}  // namespace linguine
