#include "SelectableFeatureRenderer.h"

#include <string>

#include <glm/gtc/type_ptr.hpp>

#include <renderer/features/SelectableFeature.h>

namespace linguine::render {

SelectableFeatureRenderer::SelectableFeatureRenderer(MeshRegistry& meshRegistry)
    : _meshRegistry(meshRegistry) {
  glGenTextures(1, &_targetTexture);
  glGenRenderbuffers(1, &_depthBuffer);
  glGenFramebuffers(1, &_framebuffer);

  auto result = GL_FALSE;

  auto vertexShaderId = glCreateShader(GL_VERTEX_SHADER);

  {
    auto vertexShaderCode =
        R"(#version 300 es

           layout(location = 0)
           in vec2 inPosition;

           uniform mat4 viewProjectionMatrix;
           uniform mat4 modelMatrix;

           void main() {
             gl_Position = viewProjectionMatrix * modelMatrix * vec4(inPosition, 0.0, 1.0);
           }
      )";

    glShaderSource(vertexShaderId, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShaderId);

    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &result);

    if (!result) {
      auto logLength = 0;
      glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &logLength);

      auto error = std::string();
      error.reserve(logLength + 1);

      glGetShaderInfoLog(vertexShaderId, logLength, NULL, error.data());

      printf("Vertex shader error: %s\n", error.c_str());
      throw std::runtime_error(error);
    }
  }

  auto fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

  {
    auto fragmentShaderCode =
        R"(#version 300 es

           precision highp int;

           uniform uvec4 id;

           out uvec4 fragColor;

           void main() {
             fragColor = id;
           }
      )";

    glShaderSource(fragmentShaderId, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShaderId);

    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &result);

    if (!result) {
      auto logLength = 0;
      glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &logLength);

      auto error = std::string();
      error.reserve(logLength + 1);

      glGetShaderInfoLog(fragmentShaderId, logLength, NULL, error.data());

      printf("Fragment shader error: %s\n", error.c_str());
      throw std::runtime_error(error);
    }
  }

  _shaderProgram = glCreateProgram();
  glAttachShader(_shaderProgram, vertexShaderId);
  glAttachShader(_shaderProgram, fragmentShaderId);
  glLinkProgram(_shaderProgram);

  glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &result);

  if (!result) {
    auto logLength = 0;
    glGetProgramiv(_shaderProgram, GL_INFO_LOG_LENGTH, &logLength);

    auto error = std::string();
    error.reserve(logLength + 1);

    glGetProgramInfoLog(_shaderProgram, logLength, NULL, error.data());

    printf("Program error: %s\n", error.c_str());
    throw std::runtime_error(error);
  }

  glDetachShader(_shaderProgram, vertexShaderId);
  glDetachShader(_shaderProgram, fragmentShaderId);

  glDeleteShader(vertexShaderId);
  glDeleteShader(fragmentShaderId);

  _viewProjectionMatrixLocation = glGetUniformLocation(_shaderProgram, "viewProjectionMatrix");
  _modelMatrixLocation = glGetUniformLocation(_shaderProgram, "modelMatrix");
  _idLocation = glGetUniformLocation(_shaderProgram, "id");
}

SelectableFeatureRenderer::~SelectableFeatureRenderer() {
  glDeleteProgram(_shaderProgram);
  glDeleteTextures(1, &_targetTexture);
  glDeleteRenderbuffers(1, &_depthBuffer);
  glDeleteFramebuffers(1, &_framebuffer);
}

void SelectableFeatureRenderer::resize(uint16_t width, uint16_t height) {
  _width = width;
  _height = height;

  glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

  glBindTexture(GL_TEXTURE_2D, _targetTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16UI, width, height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, nullptr);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _targetTexture, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool SelectableFeatureRenderer::isRelevant(Renderable& renderable) {
  return renderable.hasFeature<SelectableFeature>();
}

void SelectableFeatureRenderer::draw(linguine::Camera& camera) {
  glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

  if (camera.getId() == 0) {
    auto clearColor = glm::uvec4(UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX);
    glClearBufferuiv(GL_COLOR, 0, glm::value_ptr(clearColor));
  }

  glClear(GL_DEPTH_BUFFER_BIT);

  glUseProgram(_shaderProgram);

  glUniformMatrix4fv(_viewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix));

  auto filteredRenderables = std::vector<Renderable*>();

  for (const auto& renderable : getRenderables()) {
    if (renderable.second->getLayer() == camera.layer && renderable.second->isEnabled()) {
      filteredRenderables.push_back(renderable.second);
    }
  }

  for (const auto& renderable : filteredRenderables) {
    auto& feature = renderable->getFeature<SelectableFeature>();

    glUniformMatrix4fv(_modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(feature.modelMatrix));

    auto data = glm::uvec4(
        feature.entityId & 0xFFFF,
        (feature.entityId >> 16) & 0xFFFF,
        (feature.entityId >> 32) & 0xFFFF,
        (feature.entityId >> 48) & 0xFFFF
    );

    glUniform4uiv(_idLocation, 1, glm::value_ptr(data));

    auto& mesh = _meshRegistry.get(feature.meshType);
    mesh->bind();

    mesh->draw();

    mesh->unbind();
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::optional<uint64_t> SelectableFeatureRenderer::getEntityIdAt(float x, float y) const {
  glFinish();

  glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

  glm::uvec4 data;
  glReadPixels(x * _width, y * _height, 1, 1, GL_RGBA_INTEGER, GL_UNSIGNED_INT, glm::value_ptr(data));
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  auto id = static_cast<uint64_t>(data.x)
            + (static_cast<uint64_t>(data.y) << 16)
            + (static_cast<uint64_t>(data.z) << 32)
            + (static_cast<uint64_t>(data.w) << 48);

  if (id != UINT64_MAX) {
    return id;
  }

  return {};
}

}  // namespace linguine::render
