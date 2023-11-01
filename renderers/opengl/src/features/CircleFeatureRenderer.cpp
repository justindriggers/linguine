#include "CircleFeatureRenderer.h"

#include <list>
#include <string>

#include <glm/gtc/type_ptr.hpp>

#include <renderer/features/CircleFeature.h>

namespace linguine::render {

CircleFeatureRenderer::CircleFeatureRenderer(MeshRegistry &meshRegistry)
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

           out vec2 uv;

           void main() {
             gl_Position = viewProjectionMatrix * modelMatrix * vec4(inPosition, 0.0, 1.0);
             uv = inPosition;
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

           precision highp float;

           uniform vec3 color;

           in vec2 uv;

           out vec4 outColor;

           void main() {
             float distance = length(uv);
             float delta = fwidth(distance);
             float alpha = 1.0 - smoothstep(0.5 - delta, 0.5, distance);

             outColor = vec4(color, alpha);
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
  _colorLocation = glGetUniformLocation(_shaderProgram, "color");
}

CircleFeatureRenderer::~CircleFeatureRenderer() {
  glDeleteProgram(_shaderProgram);
}

bool CircleFeatureRenderer::isRelevant(Renderable& renderable) {
  return renderable.hasFeature<CircleFeature>();
}

void CircleFeatureRenderer::draw(Camera& camera) {
  glUseProgram(_shaderProgram);

  glUniformMatrix4fv(_viewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix));

  auto cameraDepth = camera.viewMatrix[3][2];

  auto filteredRenderables = std::list<Renderable*>();

  for (const auto& renderable : getRenderables()) {
    if (renderable.second->getLayer() == camera.layer && renderable.second->isEnabled()) {
      auto feature = renderable.second->getFeature<CircleFeature>();
      auto distance = glm::abs(feature.modelMatrix[3][2] - cameraDepth);

      filteredRenderables.insert(std::lower_bound(filteredRenderables.begin(), filteredRenderables.end(), distance, [cameraDepth](const Renderable* i, float value) {
        auto feature = i->getFeature<CircleFeature>();
        auto distance = glm::abs(feature.modelMatrix[3][2] - cameraDepth);
        return value < distance;
      }), renderable.second);
    }
  }

  for (const auto& renderable : filteredRenderables) {
    auto& feature = renderable->getFeature<CircleFeature>();

    glUniformMatrix4fv(_modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(feature.modelMatrix));
    glUniform3fv(_colorLocation, 1, glm::value_ptr(feature.color));

    auto& mesh = _meshRegistry.get(MeshType::Quad);
    mesh->bind();

    mesh->draw();

    mesh->unbind();
  }
}

}  // namespace linguine::render
