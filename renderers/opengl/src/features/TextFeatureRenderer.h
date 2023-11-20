#pragma once

#include <renderer/features/FeatureRenderer.h>

#include <GLES3/gl3.h>

#include "OpenGLFileLoader.h"
#include "mesh/MeshRegistry.h"

namespace linguine::render {

class TextFeatureRenderer : public FeatureRenderer {
  public:
    TextFeatureRenderer(MeshRegistry& meshRegistry,
                        OpenGLFileLoader& fileLoader);

    ~TextFeatureRenderer() override;

    bool isRelevant(Renderable& renderable) override;

    void onFrameBegin() override {}

    void draw(Camera& camera) override;

    void resize(uint16_t width, uint16_t height) override {}

    void reset() override {}

  private:
    MeshRegistry& _meshRegistry;
    GLuint _fontTexture;
    GLuint _shaderProgram;
    GLint _viewProjectionMatrixLocation;
    GLint _modelMatrixLocation;
    GLint _colorLocation;
    GLint _samplerLocation;
    GLint _positionLocation;
    GLuint _sampler;

    const std::unordered_map<char, glm::vec2> _glyphPositions = {
        { 'A', glm::vec2{ 0.0f, 0.0f } },
        { 'B', glm::vec2{ 5.0f, 0.0f } },
        { 'C', glm::vec2{ 10.0f, 0.0f } },
        { 'D', glm::vec2{ 15.0f, 0.0f } },
        { 'E', glm::vec2{ 20.0f, 0.0f } },
        { 'F', glm::vec2{ 25.0f, 0.0f } },
        { 'G', glm::vec2{ 30.0f, 0.0f } },
        { 'H', glm::vec2{ 35.0f, 0.0f } },
        { 'I', glm::vec2{ 40.0f, 0.0f } },
        { 'J', glm::vec2{ 45.0f, 0.0f } },
        { 'K', glm::vec2{ 50.0f, 0.0f } },
        { 'L', glm::vec2{ 55.0f, 0.0f } },
        { 'M', glm::vec2{ 60.0f, 0.0f } },
        { 'N', glm::vec2{ 65.0f, 0.0f } },
        { 'O', glm::vec2{ 70.0f, 0.0f } },
        { 'P', glm::vec2{ 75.0f, 0.0f } },
        { 'Q', glm::vec2{ 80.0f, 0.0f } },
        { 'R', glm::vec2{ 85.0f, 0.0f } },
        { 'S', glm::vec2{ 90.0f, 0.0f } },
        { 'T', glm::vec2{ 95.0f, 0.0f } },
        { 'U', glm::vec2{ 100.0f, 0.0f } },
        { 'V', glm::vec2{ 105.0f, 0.0f } },
        { 'W', glm::vec2{ 110.0f, 0.0f } },
        { 'X', glm::vec2{ 115.0f, 0.0f } },
        { 'Y', glm::vec2{ 120.0f, 0.0f } },
        { 'Z', glm::vec2{ 125.0f, 0.0f } },

        { 'a', glm::vec2{ 0.0f, 0.0f } },
        { 'b', glm::vec2{ 5.0f, 0.0f } },
        { 'c', glm::vec2{ 10.0f, 0.0f } },
        { 'd', glm::vec2{ 15.0f, 0.0f } },
        { 'e', glm::vec2{ 20.0f, 0.0f } },
        { 'f', glm::vec2{ 25.0f, 0.0f } },
        { 'g', glm::vec2{ 30.0f, 0.0f } },
        { 'h', glm::vec2{ 35.0f, 0.0f } },
        { 'i', glm::vec2{ 40.0f, 0.0f } },
        { 'j', glm::vec2{ 45.0f, 0.0f } },
        { 'k', glm::vec2{ 50.0f, 0.0f } },
        { 'l', glm::vec2{ 55.0f, 0.0f } },
        { 'm', glm::vec2{ 60.0f, 0.0f } },
        { 'n', glm::vec2{ 65.0f, 0.0f } },
        { 'o', glm::vec2{ 70.0f, 0.0f } },
        { 'p', glm::vec2{ 75.0f, 0.0f } },
        { 'q', glm::vec2{ 80.0f, 0.0f } },
        { 'r', glm::vec2{ 85.0f, 0.0f } },
        { 's', glm::vec2{ 90.0f, 0.0f } },
        { 't', glm::vec2{ 95.0f, 0.0f } },
        { 'u', glm::vec2{ 100.0f, 0.0f } },
        { 'v', glm::vec2{ 105.0f, 0.0f } },
        { 'w', glm::vec2{ 110.0f, 0.0f } },
        { 'x', glm::vec2{ 115.0f, 0.0f } },
        { 'y', glm::vec2{ 120.0f, 0.0f } },
        { 'z', glm::vec2{ 125.0f, 0.0f } },

        { '0', glm::vec2{ 0.0f, 6.0f } },
        { '1', glm::vec2{ 5.0f, 6.0f } },
        { '2', glm::vec2{ 10.0f, 6.0f } },
        { '3', glm::vec2{ 15.0f, 6.0f } },
        { '4', glm::vec2{ 20.0f, 6.0f } },
        { '5', glm::vec2{ 25.0f, 6.0f } },
        { '6', glm::vec2{ 30.0f, 6.0f } },
        { '7', glm::vec2{ 35.0f, 6.0f } },
        { '8', glm::vec2{ 40.0f, 6.0f } },
        { '9', glm::vec2{ 45.0f, 6.0f } },

        { '.', glm::vec2{ 50.0f, 6.0f } },
        { ',', glm::vec2{ 55.0f, 6.0f } },
        { '!', glm::vec2{ 60.0f, 6.0f } },
        { '?', glm::vec2{ 65.0f, 6.0f } },
        { '(', glm::vec2{ 70.0f, 6.0f } },
        { ')', glm::vec2{ 75.0f, 6.0f } },
        { '+', glm::vec2{ 80.0f, 6.0f } },
        { '-', glm::vec2{ 85.0f, 6.0f } },
        { '=', glm::vec2{ 90.0f, 6.0f } },
        { '/', glm::vec2{ 95.0f, 6.0f } },
        { '\'', glm::vec2{ 100.0f, 6.0f } },
        { ':', glm::vec2{ 105.0f, 6.0f } },
        { ' ', glm::vec2{ 110.0f, 6.0f } }
    };

    [[nodiscard]] glm::vec2 getGlyphPosition(char character) const {
      auto result = _glyphPositions.find(character);

      if (result != _glyphPositions.end()) {
        return result->second;
      }

      return glm::vec2{ 115.0f, 6.0f };
    }
};

}  // namespace linguine::render
