#include "TextFeatureRenderer.h"

#include <string>

#include <glm/gtc/type_ptr.hpp>

#include <renderer/features/TextFeature.h>

namespace linguine::render {

TextFeatureRenderer::TextFeatureRenderer(MeshRegistry &meshRegistry,
                                         OpenGLFileLoader& fileLoader)
    : _meshRegistry(meshRegistry) {
  auto fontImage = fileLoader.getImage();

  glGenTextures(1, &_fontTexture);
  glBindTexture(GL_TEXTURE_2D, _fontTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, fontImage.width, fontImage.height, 0, GL_RGB, GL_UNSIGNED_BYTE, fontImage.data.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);

  auto result = GL_FALSE;

  auto vertexShaderId = glCreateShader(GL_VERTEX_SHADER);

  {
    auto vertexShaderCode =
        R"(#version 300 es

           layout(location = 0)
           in vec2 inPosition;

           out vec2 uv;

           uniform mat4 viewProjectionMatrix;
           uniform mat4 modelMatrix;

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
           precision highp sampler2D;

           uniform vec3 color;
           uniform sampler2D sampler;
           uniform vec2 position;

           in vec2 uv;

           out vec4 outColor;

           void main() {
             float x = (position.x + floor(10.0 * (uv.x + 0.5)) + 0.5) / 160.0;
             float y = (position.y + floor(10.0 * (-uv.y + 0.5)) + 0.5) / 50.0;
             float a = 1.0 - texture(sampler, vec2(x, y)).r;
             outColor = vec4(color, a);
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
  _samplerLocation = glGetUniformLocation(_shaderProgram, "sampler");
  _positionLocation = glGetUniformLocation(_shaderProgram, "position");

  glGenSamplers(1, &_sampler);
  glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

TextFeatureRenderer::~TextFeatureRenderer() {
  glDeleteSamplers(1, &_sampler);
  glDeleteProgram(_shaderProgram);
}

bool TextFeatureRenderer::isRelevant(Renderable& renderable) {
  return renderable.hasFeature<TextFeature>();
}

void TextFeatureRenderer::draw(Camera& camera) {
  glUseProgram(_shaderProgram);

  glUniformMatrix4fv(_viewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.viewProjectionMatrix));

  auto& mesh = _meshRegistry.get(MeshType::Quad);
  mesh->bind();

  glUniform1i(_samplerLocation, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _fontTexture);
  glBindSampler(0, _sampler);

  auto filteredRenderables = std::vector<Renderable*>();

  for (const auto& renderable : getRenderables()) {
    if (renderable.second->getLayer() == camera.layer && renderable.second->isEnabled()) {
      filteredRenderables.push_back(renderable.second);
    }
  }

  for (const auto& renderable : filteredRenderables) {
    auto& feature = renderable->getFeature<TextFeature>();

    glUniform3fv(_colorLocation, 1, glm::value_ptr(feature.color));

    auto currentModelMatrix = feature.modelMatrix;
    auto lineStartModelMatrix = currentModelMatrix;
    auto horizontalTranslation = glm::vec3(1.0f, 0.0f, 0.0f);
    auto verticalTranslation = glm::vec3(0.0f, -1.0f, 0.0f);

    for (const auto& character : feature.text) {
      if (character == '\n') {
        currentModelMatrix = glm::translate(lineStartModelMatrix, verticalTranslation);
        lineStartModelMatrix = currentModelMatrix;
      } else {
        glUniformMatrix4fv(_modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(currentModelMatrix));
        glUniform2fv(_positionLocation, 1, glm::value_ptr(_glyphPositions.at(character)));

        mesh->draw();

        currentModelMatrix = glm::translate(currentModelMatrix, horizontalTranslation);
      }
    }
  }

  glBindSampler(0, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  mesh->unbind();
}

}  // namespace linguine::render
