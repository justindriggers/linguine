#include "OpenGLRenderer.h"

#include <GLES3/gl3.h>

#include "features/ColoredFeatureRenderer.h"
#include "features/ProgressFeatureRenderer.h"

namespace linguine::render {

class OpenGLRendererImpl : public OpenGLRenderer {
  public:
    OpenGLRendererImpl();

    ~OpenGLRendererImpl() override = default;

    void draw() override;

    [[nodiscard]] std::optional<uint64_t> getEntityIdAt(float x, float y) const override {
      return {};
    }

  protected:
    [[nodiscard]] const std::vector<std::unique_ptr<FeatureRenderer>>& getFeatures() const override {
      return _features;
    }

  private:
    std::vector<std::unique_ptr<FeatureRenderer>> _features;
    MeshRegistry _meshRegistry;
};

OpenGLRendererImpl::OpenGLRendererImpl() {
  _features.push_back(std::make_unique<ColoredFeatureRenderer>(_meshRegistry));
  _features.push_back(std::make_unique<ProgressFeatureRenderer>(_meshRegistry));
  _features.shrink_to_fit();

  glFrontFace(GL_CCW);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
}

void OpenGLRendererImpl::draw() {
  for (const auto& camera : getCameras()) {
    if (camera->clearColor) {
      glClearColor(camera->clearColor->r, camera->clearColor->g, camera->clearColor->b, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
    }

    glClearDepthf(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (const auto& feature : getFeatures()) {
      feature->draw(*camera);
    }
  }
}

OpenGLRenderer* OpenGLRenderer::create() {
  return new OpenGLRendererImpl();
}

}  // namespace linguine::render
