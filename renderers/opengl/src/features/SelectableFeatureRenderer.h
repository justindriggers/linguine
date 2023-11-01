#pragma once

#include <renderer/features/FeatureRenderer.h>

#include <GLES3/gl3.h>

#include "mesh/MeshRegistry.h"

namespace linguine::render {

class SelectableFeatureRenderer : public FeatureRenderer {
  public:
    explicit SelectableFeatureRenderer(MeshRegistry& meshRegistry);

    ~SelectableFeatureRenderer() override;

    bool isRelevant(Renderable& renderable) override;

    void onFrameBegin() override;

    void draw(Camera& camera) override;

    void resize(uint16_t width, uint16_t height) override;

    std::optional<uint64_t> getEntityIdAt(float x, float y) const;

  private:
    MeshRegistry& _meshRegistry;

    GLuint _targetTexture;
    GLuint _depthBuffer;
    GLuint _framebuffer;

    GLuint _shaderProgram;
    GLint _viewProjectionMatrixLocation;
    GLint _modelMatrixLocation;
    GLint _idLocation;

    uint16_t _width;
    uint16_t _height;
};

}  // namespace linguine::render
