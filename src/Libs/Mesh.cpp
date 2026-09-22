#include "Mesh.h"

#include <cstddef>
#include <utility>

Mesh::Mesh()
    : VAO(0), VBO(0), IBO(0), indexCount(0)
{
}

Mesh::~Mesh()
{
    ClearMesh();
}

Mesh::Mesh(Mesh&& other) noexcept
    : VAO(std::exchange(other.VAO, 0)),
      VBO(std::exchange(other.VBO, 0)),
      IBO(std::exchange(other.IBO, 0)),
      indexCount(std::exchange(other.indexCount, 0))
{
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other)
    {
        ClearMesh();
        VAO = std::exchange(other.VAO, 0);
        VBO = std::exchange(other.VBO, 0);
        IBO = std::exchange(other.IBO, 0);
        indexCount = std::exchange(other.indexCount, 0);
    }
    return *this;
}

bool Mesh::CreateMesh(const std::vector<Vertex>& vertices,
                      const std::vector<unsigned int>& indices)
{
    if (vertices.empty() || indices.empty())
        return false;

    ClearMesh();
    indexCount = static_cast<GLsizei>(indices.size());

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indices.size() * sizeof(indices[0])),
                 indices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(vertices[0])),
                 vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, x)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, u)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, nx)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return glGetError() == GL_NO_ERROR;
}

void Mesh::RenderMesh() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::ClearMesh()
{
    if (IBO != 0)
    {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }

    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    indexCount = 0;
}
