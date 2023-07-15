#pragma once

#include <GLES3/gl3.h>

#include "mesh/ScreenMesh.h"

namespace linguine::render {

class GammaCorrection {
  public:
    explicit GammaCorrection(GLuint sourceTexture);

    ~GammaCorrection();

    void draw();

  private:
    GLuint _sourceTexture;

    ScreenMesh _screenMesh;

    GLuint _shaderProgram;
    GLint _sourceLocation;
};

}  // namespace linguine::render
