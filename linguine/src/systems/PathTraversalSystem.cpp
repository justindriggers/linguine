#include "PathTraversalSystem.h"

#include <glm/gtx/compatibility.hpp>

#include "components/Path.h"
#include "components/Transform.h"

namespace linguine {

void PathTraversalSystem::update(float deltaTime) {
  findEntities<Path, Transform>()->each([deltaTime](const Entity& entity) {
    auto path = entity.get<Path>();
    path->t += path->speed * deltaTime;

    while (path->t >= 1.0f) {
      path->t -= 1.0f;

      if (++path->index >= path->spline.curves.size()) {
        path->index = 0;
      }
    }

    auto curve = path->spline.curves[path->index];

    auto a = glm::lerp(curve.points[0], curve.points[1], path->t);
    auto b = glm::lerp(curve.points[1], curve.points[2], path->t);
    auto c = glm::lerp(curve.points[2], curve.points[3], path->t);

    auto d = glm::lerp(a, b, path->t);
    auto e = glm::lerp(b, c, path->t);

    auto position = glm::lerp(d, e, path->t) * path->scale;

    auto transform = entity.get<Transform>();
    transform->position.x = position.x;
    transform->position.y = position.y;
  });
}

}  // namespace linguine
