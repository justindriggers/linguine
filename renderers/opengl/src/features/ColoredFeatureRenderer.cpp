#include "ColoredFeatureRenderer.h"

#include <string>

#include <glm/gtc/type_ptr.hpp>

#include <renderer/features/ColoredFeature.h>

namespace linguine::render {

ColoredFeatureRenderer::ColoredFeatureRenderer(MeshRegistry &meshRegistry)
    : _meshRegistry(meshRegistry) {
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

           precision mediump float;

           uniform vec3 color;

           out vec4 outColor;

           void main() {
             outColor = vec4(color, 1.0);
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

      glGetShaderInfoLog(vertexShaderId, logLength, NULL, error.data());

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
  _colorLocation = glGetUniformLocation(_shaderProgram, "color");
}

ColoredFeatureRenderer::~ColoredFeatureRenderer() {
  glDeleteProgram(_shaderProgram);
}

bool ColoredFeatureRenderer::isRelevant(Renderable& renderable) {
  auto result = renderable.hasFeature<ColoredFeature>();
  printf("%llu: %b\n", renderable.getId(), result);
  return result;
}

void ColoredFeatureRenderer::draw(Camera& camera) {
  glUseProgram(_shaderProgram);

  glUniformMatrix4fv(_viewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix));

  auto filteredRenderables = std::vector<Renderable*>();

  for (const auto& renderable : getRenderables()) {
    if (renderable.second->getLayer() == camera.layer && renderable.second->isEnabled()) {
      filteredRenderables.push_back(renderable.second);
    }
  }

  for (const auto& renderable : filteredRenderables) {
    auto& feature = renderable->getFeature<ColoredFeature>();

    glUniformMatrix4fv(_modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(feature.modelMatrix));
    glUniform3fv(_colorLocation, 1, glm::value_ptr(feature.color));

    auto& mesh = _meshRegistry.get(feature.meshType);
    mesh->bind();

    mesh->draw();

    mesh->unbind();
  }
}

}  // namespace linguine::render
