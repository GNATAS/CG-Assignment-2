#ifndef MESH____H
#define MESH____H

#include <GL/glew.h>

#include <vector>

struct Vertex
{
    GLfloat x, y, z;
    GLfloat u, v;
    GLfloat nx, ny, nz;
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    bool CreateMesh(const std::vector<Vertex>& vertices,
                    const std::vector<unsigned int>& indices);
    void RenderMesh() const;
    void ClearMesh();

private:
    GLuint VAO, VBO, IBO;
    GLsizei indexCount;
};

#endif
