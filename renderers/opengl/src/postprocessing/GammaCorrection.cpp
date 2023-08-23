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

           const float dotSize = 8.0;

           in vec2 uv;

           uniform sampler2D source;

           out vec4 outColor;

           vec2 grid(vec2 px, float size) {
             return px - mod(px, size);
           }

           vec2 rot(vec2 p, float deg) {
             float c = cos(deg);
             float s = sin(deg);
             mat2 m = mat2(c, s, s, -c);
             return m*p;
           }

           vec3 halftone(ivec2 resolution, vec2 coord, float rotation,
                         vec2 offset, vec3 components, vec3 color) {
             vec2 gridEdge = grid(rot(coord, rotation), dotSize);
             vec2 center = rot(gridEdge + 0.5 * dotSize, rotation);
             vec4 value = texture(source, center / vec2(resolution));
             value = vec4(pow(value.rgb, vec3(1.0 / 2.2)), value.a);

             vec3 layer = value.rgb * components;

             float distance = length(coord - center + offset);
             float brightness = max(layer.r, max(layer.g, layer.b));

             if (distance < dotSize * brightness * 0.5) {
               color += layer;
             }

             return color;
           }

           void main() {
             ivec2 resolution = textureSize(source, 0);
             vec2 coord = uv * vec2(resolution);

             float offset = 1.0;
             vec3 color = vec3(0.0);

             color = halftone(resolution, coord, 15.0, vec2(-offset, offset), vec3(0.0, 1.0, 1.0), color);
             color = halftone(resolution, coord, 75.0, vec2(offset, offset), vec3(1.0, 0.0, 1.0), color);
             color = halftone(resolution, coord, 0.0, vec2(-offset, -offset), vec3(1.0, 1.0, 0.0), color);

             bool foreground = length(color) > 0.0;

             float rotation = 45.0;

             vec2 gridEdge = grid(rot(coord, rotation), dotSize);
             vec2 center = rot(gridEdge + 0.5 * dotSize, rotation);
             vec4 value = texture(source, center / vec2(resolution));
             value = vec4(pow(value.rgb, vec3(1.0 / 2.2)), value.a);

             float kDistance = length(center - coord);
             float brightness = max(value.r, max(value.g, value.b));

             if (kDistance < dotSize * (1.0 - brightness) * 0.5) {
               color = value.rgb;
               foreground = true;
             }

             if (!foreground)
               color = vec3(1.0);

             outColor = vec4(color, value.a);
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
