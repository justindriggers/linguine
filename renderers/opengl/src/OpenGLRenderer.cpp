#include "OpenGLRenderer.h"

#include <GLES3/gl3.h>

namespace linguine::render {

void OpenGLRenderer::draw() {
  for (const auto& camera : getCameras()) {
    if (camera->clearColor) {
      glClearColor(camera->clearColor->r, camera->clearColor->g, camera->clearColor->b, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
    }
  }
}

}  // namespace linguine::render
