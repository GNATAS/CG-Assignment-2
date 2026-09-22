#ifndef TEXTURE____H
#define TEXTURE____H

#include <GL/glew.h>

#include <filesystem>

class Texture
{
public:
    Texture();
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    bool Load(const std::filesystem::path& path);
    void Bind(GLenum textureUnit = GL_TEXTURE0) const;
    void Clear();

    bool IsLoaded() const { return textureId != 0; }

private:
    GLuint textureId;
};

#endif
