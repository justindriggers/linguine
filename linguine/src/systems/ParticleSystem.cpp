#include "ParticleSystem.h"

#include <glm/gtx/norm.hpp>

#include "components/Emitter.h"
#include "components/Particle.h"
#include "components/Transform.h"

namespace linguine {

void ParticleSystem::update(float deltaTime) {
  findEntities<Emitter>()->each([deltaTime](const Entity& entity) {
    auto emitter = entity.get<Emitter>();

    emitter->elapsed += deltaTime;

    if (emitter->elapsed >= emitter->frequency) {
      emitter->elapsed -= emitter->frequency;
      emitter->particleFactory();
    }
  });

  findEntities<Particle, Transform>()->each([deltaTime](Entity& entity) {
    auto particle = entity.get<Particle>();

    particle->elapsed += deltaTime;

    auto transform = entity.get<Transform>();
    transform->scale = glm::max(transform->scale - glm::vec3(0.25f) * deltaTime, glm::vec3(0.0f));

    if (particle->elapsed >= particle->duration || glm::length2(transform->scale) <= 0.0f) {
      entity.destroy();
    }
  });
}

}  // namespace linguine
