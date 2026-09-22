#include "Model.h"

#include <tiny_obj_loader.h>

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace
{
struct IndexKey
{
    int vertex;
    int texcoord;
    int normal;

    bool operator==(const IndexKey& other) const
    {
        return vertex == other.vertex &&
               texcoord == other.texcoord &&
               normal == other.normal;
    }
};

struct IndexKeyHash
{
    std::size_t operator()(const IndexKey& key) const
    {
        std::size_t value = std::hash<int>{}(key.vertex);
        value ^= std::hash<int>{}(key.texcoord) + 0x9e3779b9 + (value << 6) + (value >> 2);
        value ^= std::hash<int>{}(key.normal) + 0x9e3779b9 + (value << 6) + (value >> 2);
        return value;
    }
};
}

bool Model::Load(const std::filesystem::path& objPath)
{
    tinyobj::ObjReaderConfig config;
    config.mtl_search_path = objPath.parent_path().u8string();
    config.triangulate = true;

    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(objPath.u8string(), config))
    {
        if (!reader.Error().empty())
            std::cerr << reader.Error() << '\n';
        std::cerr << "Failed to load model: " << objPath << '\n';
        return false;
    }
    if (!reader.Warning().empty())
        std::cerr << "Model warning for " << objPath.filename() << ": "
                  << reader.Warning() << '\n';

    const tinyobj::attrib_t& attributes = reader.GetAttrib();
    const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
    const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();

    std::size_t cornerCount = 0;
    for (const tinyobj::shape_t& shape : shapes)
        cornerCount += shape.mesh.indices.size();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve(attributes.vertices.size() / 3);
    indices.reserve(cornerCount);

    std::unordered_map<IndexKey, unsigned int, IndexKeyHash> vertexLookup;
    vertexLookup.reserve(attributes.vertices.size() / 3);
    std::unordered_set<int> usedMaterials;

    const float highest = std::numeric_limits<float>::max();
    bounds.minimum = glm::vec3(highest);
    bounds.maximum = glm::vec3(-highest);

    for (const tinyobj::shape_t& shape : shapes)
    {
        for (int materialId : shape.mesh.material_ids)
        {
            if (materialId >= 0)
                usedMaterials.insert(materialId);
        }

        for (const tinyobj::index_t& index : shape.mesh.indices)
        {
            const IndexKey key{index.vertex_index, index.texcoord_index, index.normal_index};
            const auto existing = vertexLookup.find(key);
            if (existing != vertexLookup.end())
            {
                indices.push_back(existing->second);
                continue;
            }

            if (index.vertex_index < 0 ||
                static_cast<std::size_t>(3 * index.vertex_index + 2) >= attributes.vertices.size())
            {
                std::cerr << "Invalid position index in model: " << objPath << '\n';
                return false;
            }

            Vertex vertex{};
            vertex.x = attributes.vertices[3 * index.vertex_index];
            vertex.y = attributes.vertices[3 * index.vertex_index + 1];
            vertex.z = attributes.vertices[3 * index.vertex_index + 2];

            if (index.texcoord_index >= 0 &&
                static_cast<std::size_t>(2 * index.texcoord_index + 1) < attributes.texcoords.size())
            {
                vertex.u = attributes.texcoords[2 * index.texcoord_index];
                vertex.v = attributes.texcoords[2 * index.texcoord_index + 1];
            }

            if (index.normal_index >= 0 &&
                static_cast<std::size_t>(3 * index.normal_index + 2) < attributes.normals.size())
            {
                vertex.nx = attributes.normals[3 * index.normal_index];
                vertex.ny = attributes.normals[3 * index.normal_index + 1];
                vertex.nz = attributes.normals[3 * index.normal_index + 2];
            }
            else
            {
                vertex.ny = 1.0f;
            }

            const glm::vec3 position(vertex.x, vertex.y, vertex.z);
            bounds.minimum = glm::min(bounds.minimum, position);
            bounds.maximum = glm::max(bounds.maximum, position);

            const unsigned int newIndex = static_cast<unsigned int>(vertices.size());
            vertices.push_back(vertex);
            vertexLookup.emplace(key, newIndex);
            indices.push_back(newIndex);
        }
    }

    if (usedMaterials.size() > 1)
    {
        std::cerr << "Model uses multiple materials; this assignment loader uses the first: "
                  << objPath << '\n';
    }

    int materialId = usedMaterials.empty() ? (materials.empty() ? -1 : 0) : *usedMaterials.begin();
    if (materialId < 0 || static_cast<std::size_t>(materialId) >= materials.size() ||
        materials[materialId].diffuse_texname.empty())
    {
        std::cerr << "Model has no diffuse texture in its MTL: " << objPath << '\n';
        return false;
    }

    const std::filesystem::path texturePath =
        objPath.parent_path() / std::filesystem::u8path(materials[materialId].diffuse_texname);
    if (!texture.Load(texturePath) || !mesh.CreateMesh(vertices, indices))
        return false;

    std::cout << "Loaded model " << objPath.filename()
              << ": " << vertices.size() << " GPU vertices, "
              << indices.size() / 3 << " triangles, bounds ["
              << bounds.minimum.x << ", " << bounds.minimum.y << ", " << bounds.minimum.z
              << "] to [" << bounds.maximum.x << ", " << bounds.maximum.y << ", "
              << bounds.maximum.z << "]\n";
    return true;
}

void Model::Render() const
{
    texture.Bind();
    mesh.RenderMesh();
}
