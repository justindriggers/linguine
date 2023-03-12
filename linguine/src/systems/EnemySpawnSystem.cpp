#include "EnemySpawnSystem.h"

#include "components/Alive.h"
#include "components/Health.h"
#include "components/Hostile.h"
#include "components/Progressable.h"
#include "components/Transform.h"

namespace linguine {

void EnemySpawnSystem::update(float deltaTime) {
  auto alive = findEntities<Hostile, Alive>()->get();

  if (alive.empty()) {
    findEntities<Hostile>()->each([](Entity& entity) {
      entity.destroy();
    });

    int enemyCount = ++_wave;

    for (int i = 0; i < enemyCount; ++i) {
      createEnemy({-static_cast<float>(enemyCount - 1) / 2.0f * 1.5f + static_cast<float>(i) * 1.5f, 3.0f, 0.0f});
    }
  }
}

void EnemySpawnSystem::createEnemy(glm::vec3 location) {
  auto enemy = createEntity();
  enemy->add<Hostile>();

  auto transform = enemy->add<Transform>();
  transform->position = location;

  auto progressable = enemy->add<Progressable>();
  progressable->feature = new ProgressFeature();
  progressable->feature->meshType = Triangle;
  progressable->feature->color = glm::vec3(1.0f, 0.0f, 0.0f);
  progressable->renderable = _renderer.create(std::unique_ptr<ProgressFeature>(progressable->feature));
  progressable.setRemovalListener([progressable](const Entity e) {
    progressable->renderable->destroy();
  });

  auto health = enemy->add<Health>();
  health->current = 10'000;
  health->max = 10'000;

  enemy->add<Alive>();
}

}  // namespace linguine
