#include "OpenGLRenderer.h"

#include <GLES3/gl3.h>

#include "features/CircleFeatureRenderer.h"
#include "features/ColoredFeatureRenderer.h"
#include "features/ProgressFeatureRenderer.h"
#include "features/SelectableFeatureRenderer.h"
#include "features/TextFeatureRenderer.h"
#include "postprocessing/GammaCorrection.h"

namespace linguine::render {

class OpenGLRendererImpl : public OpenGLRenderer {
  public:
    OpenGLRendererImpl(std::unique_ptr<OpenGLFileLoader> fileLoader);

    ~OpenGLRendererImpl() override;

    void draw() override;

    void resize(uint16_t width, uint16_t height) override;

    [[nodiscard]] std::optional<uint64_t> getEntityIdAt(float x, float y) const override;

  protected:
    [[nodiscard]] const std::vector<std::unique_ptr<FeatureRenderer>>& getFeatures() const override {
      return _features;
    }

  private:
    std::unique_ptr<OpenGLFileLoader> _fileLoader;

    GLuint _targetTexture;
    GLuint _depthBuffer;
    GLuint _framebuffer;

    std::vector<std::unique_ptr<FeatureRenderer>> _features;
    SelectableFeatureRenderer* _selectableFeatureRenderer;
    MeshRegistry _meshRegistry;

    std::unique_ptr<GammaCorrection> _gammaCorrection;
};

OpenGLRendererImpl::OpenGLRendererImpl(std::unique_ptr<OpenGLFileLoader> fileLoader)
    : _selectableFeatureRenderer(new SelectableFeatureRenderer(_meshRegistry)),
      _fileLoader(std::move(fileLoader)) {
  glGenTextures(1, &_targetTexture);
  glGenRenderbuffers(1, &_depthBuffer);
  glGenFramebuffers(1, &_framebuffer);

  _features.push_back(std::make_unique<ColoredFeatureRenderer>(_meshRegistry));
  _features.push_back(std::make_unique<ProgressFeatureRenderer>(_meshRegistry));
  _features.push_back(std::unique_ptr<SelectableFeatureRenderer>(_selectableFeatureRenderer));
  _features.push_back(std::make_unique<TextFeatureRenderer>(_meshRegistry, *_fileLoader));
  _features.push_back(std::make_unique<CircleFeatureRenderer>(_meshRegistry));
  _features.shrink_to_fit();

  _gammaCorrection = std::make_unique<GammaCorrection>(_targetTexture);

  glFrontFace(GL_CCW);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquation(GL_FUNC_ADD);
}

OpenGLRendererImpl::~OpenGLRendererImpl() {
  glDeleteTextures(1, &_targetTexture);
  glDeleteRenderbuffers(1, &_depthBuffer);
  glDeleteFramebuffers(1, &_framebuffer);
}

void OpenGLRendererImpl::draw() {
  for (const auto& feature : getFeatures()) {
    feature->onFrameBegin();
  }

  for (const auto& camera : getCameras()) {
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    if (camera->clearColor) {
      glClearColor(camera->clearColor->r, camera->clearColor->g, camera->clearColor->b, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
    }

    glClearDepthf(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (const auto& feature : getFeatures()) {
      glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
      feature->draw(*camera);
    }
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  _gammaCorrection->draw();
}

void OpenGLRendererImpl::resize(uint16_t width, uint16_t height) {
  OpenGLRenderer::resize(width, height);

  glBindTexture(GL_TEXTURE_2D, _targetTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _targetTexture, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::optional<uint64_t> OpenGLRendererImpl::getEntityIdAt(float x, float y) const {
  return _selectableFeatureRenderer->getEntityIdAt(x, y);
}

OpenGLRenderer* OpenGLRenderer::create(std::unique_ptr<OpenGLFileLoader> fileLoader) {
  return new OpenGLRendererImpl(std::move(fileLoader));
}

}  // namespace linguine::render
