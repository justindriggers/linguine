#include "renderer/Camera.h"

#include "renderer/Renderer.h"

namespace linguine {

void Camera::destroy() {
  _renderer.onDestroy(*this);
}

}  // namespace linguine
