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

private:
    Mesh mesh;
    Texture texture;
    ModelBounds bounds;
};

#endif
