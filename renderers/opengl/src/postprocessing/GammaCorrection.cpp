#include "GammaCorrection.h"

#include <string>

namespace linguine::render {

GammaCorrection::GammaCorrection(GLuint sourceTexture)
    : _sourceTexture(sourceTexture) {
  auto result = GL_FALSE;

  auto vertexShaderId = glCreateShader(GL_VERTEX_SHADER);

  {
    auto vertexShaderCode =
        R"(#version 300 es

           layout(location = 0)
           in vec2 inPosition;

           out vec2 uv;

           void main() {
             gl_Position = vec4(inPosition, 0.0, 1.0);
             uv = (inPosition + 1.0) / 2.0;
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

           in vec2 uv;

           uniform sampler2D source;

           out vec4 outColor;

           void main() {
             vec4 value = texture(source, uv);
             outColor = vec4(pow(value.rgb, vec3(1.0 / 2.2)), value.a);
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

  _sourceLocation = glGetUniformLocation(_shaderProgram, "source");
}

GammaCorrection::~GammaCorrection() {
  glDeleteProgram(_shaderProgram);
}

void GammaCorrection::draw() {
  glUseProgram(_shaderProgram);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _sourceTexture);
  glUniform1i(_sourceLocation, 0);

  _screenMesh.bind();
  _screenMesh.draw();
  _screenMesh.unbind();

  glUseProgram(0);
}

}  // namespace linguine::render
