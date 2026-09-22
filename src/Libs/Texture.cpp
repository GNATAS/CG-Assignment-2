#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <utility>

Texture::Texture()
    : textureId(0)
{
}

Texture::~Texture()
{
    Clear();
}

Texture::Texture(Texture&& other) noexcept
    : textureId(std::exchange(other.textureId, 0))
{
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other)
    {
        Clear();
        textureId = std::exchange(other.textureId, 0);
    }
    return *this;
}

bool Texture::Load(const std::filesystem::path& path)
{
    Clear();
    stbi_set_flip_vertically_on_load(1);

    int width = 0;
    int height = 0;
    int channels = 0;
    const std::string pathString = path.u8string();
    unsigned char* pixels = stbi_load(pathString.c_str(), &width, &height, &channels, 0);
    if (pixels == nullptr)
    {
        std::cerr << "Failed to load texture: " << path
                  << " (" << stbi_failure_reason() << ")\n";
        return false;
    }

    GLenum format = GL_RGBA;
    switch (channels)
    {
        case 1: format = GL_RED; break;
        case 2: format = GL_RG; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default:
            std::cerr << "Unsupported channel count in texture: " << path << '\n';
            stbi_image_free(pixels);
            return false;
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height,
                 0, format, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(pixels);

    if (glGetError() != GL_NO_ERROR)
    {
        std::cerr << "OpenGL rejected texture: " << path << '\n';
        Clear();
        return false;
    }

    std::cout << "Loaded texture " << path.filename() << " ("
              << width << 'x' << height << ", " << channels << " channels)\n";
    return true;
}

void Texture::Bind(GLenum textureUnit) const
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void Texture::Clear()
{
    if (textureId != 0)
    {
        glDeleteTextures(1, &textureId);
        textureId = 0;
    }
}
