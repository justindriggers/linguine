#include "OpenGLRenderBackend.h"

#include <GLES3/gl3.h>

#include "features/CircleFeatureRenderer.h"
#include "features/ColoredFeatureRenderer.h"
#include "features/ProgressFeatureRenderer.h"
#include "features/SelectableFeatureRenderer.h"
#include "features/TextFeatureRenderer.h"
#include "postprocessing/GammaCorrection.h"

namespace linguine::render {

class OpenGLRenderBackendImpl : public OpenGLRenderBackend {
  public:
    explicit OpenGLRenderBackendImpl(std::unique_ptr<OpenGLFileLoader> fileLoader);

    ~OpenGLRenderBackendImpl() override;

    void draw(const std::vector<std::unique_ptr<Camera>>& cameras) override;

    void resize(uint16_t width, uint16_t height) override;

    void reset() override;

    [[nodiscard]] std::optional<uint64_t> getEntityIdAt(float x, float y) const override;

  protected:
    [[nodiscard]] const std::vector<std::unique_ptr<FeatureRenderer>>& getFeatures() const override {
      return _features;
    }

  private:
    std::unique_ptr<OpenGLFileLoader> _fileLoader;

    GLuint _targetTexture{};
    GLuint _depthBuffer{};
    GLuint _framebuffer{};

    uint16_t _width{};
    uint16_t _height{};

    std::vector<std::unique_ptr<FeatureRenderer>> _features;
    SelectableFeatureRenderer* _selectableFeatureRenderer{};
    bool _isFirstFrame = true;
    MeshRegistry _meshRegistry;

    std::unique_ptr<GammaCorrection> _gammaCorrection;
};

OpenGLRenderBackendImpl::OpenGLRenderBackendImpl(std::unique_ptr<OpenGLFileLoader> fileLoader)
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
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquation(GL_FUNC_ADD);
}

OpenGLRenderBackendImpl::~OpenGLRenderBackendImpl() {
  glDeleteTextures(1, &_targetTexture);
  glDeleteRenderbuffers(1, &_depthBuffer);
  glDeleteFramebuffers(1, &_framebuffer);
}

void OpenGLRenderBackendImpl::draw(const std::vector<std::unique_ptr<Camera>>& cameras) {
  for (const auto& feature : getFeatures()) {
    feature->onFrameBegin();
  }

  for (const auto& camera : cameras) {
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

  _isFirstFrame = false;
}

void OpenGLRenderBackendImpl::resize(uint16_t width, uint16_t height) {
  _width = width;
  _height = height;

  glViewport(0, 0, width, height);

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

  for (auto& feature : _features) {
    feature->resize(width, height);
  }
}

void OpenGLRenderBackendImpl::reset() {
  _isFirstFrame = true;
}

std::optional<uint64_t> OpenGLRenderBackendImpl::getEntityIdAt(float x, float y) const {
  if (_isFirstFrame || _width == 0 || _height == 0) {
    return {};
  }

  return _selectableFeatureRenderer->getEntityIdAt(x, y);
}

std::unique_ptr<OpenGLRenderBackend> OpenGLRenderBackend::create(std::unique_ptr<OpenGLFileLoader> fileLoader) {
  return std::make_unique<OpenGLRenderBackendImpl>(std::move(fileLoader));
}

}  // namespace linguine::render
