#ifndef MODEL____H
#define MODEL____H

#include <glm/glm.hpp>

#include <filesystem>

#include "Mesh.h"
#include "Texture.h"

struct ModelBounds
{
    glm::vec3 minimum{0.0f};
    glm::vec3 maximum{0.0f};
};

class Model
{
public:
    bool Load(const std::filesystem::path& objPath);
    void Render() const;

    const ModelBounds& GetBounds() const { return bounds; }
    bool HasTexture() const { return hasTexture; }
    const glm::vec3& GetDiffuseColour() const { return diffuseColour; }

private:
    Mesh mesh;
    Texture texture;
    ModelBounds bounds;
    bool hasTexture = false;
    glm::vec3 diffuseColour{1.0f};
};

#endif
