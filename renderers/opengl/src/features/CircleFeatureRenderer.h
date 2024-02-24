#pragma once

#include <renderer/features/FeatureRenderer.h>

#include <GLES3/gl3.h>

#include "mesh/MeshRegistry.h"

namespace linguine::render {

class CircleFeatureRenderer : public FeatureRenderer {
  public:
    explicit CircleFeatureRenderer(MeshRegistry& meshRegistry);

    ~CircleFeatureRenderer() override;

    bool isRelevant(Renderable& renderable) override;

    void onFrameBegin() override {}

    void draw(Camera& camera) override;

    void resize(uint16_t width, uint16_t height) override {}

    void reset() override {}

  private:
    MeshRegistry& _meshRegistry;
    GLuint _shaderProgram;
    GLint _viewProjectionMatrixLocation;
    GLint _modelMatrixLocation;
    GLint _colorLocation;
};

}  // namespace linguine::render
