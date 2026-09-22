#undef GLFW_DLL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Libs/Mesh.h"
#include "Libs/Model.h"
#include "Libs/Shader.h"
#include "Libs/Texture.h"
#include "Libs/Window.h"
#include "ProjectPaths.h"

namespace
{
struct SceneTransform
{
    glm::vec3 position;
    glm::vec3 scale;
    float yawDegrees;
};

struct Material
{
    glm::vec3 colour;
    bool useTexture;
    float emissive;
    float specular;
    float shininess;
};

struct ShaderUniforms
{
    GLint model;
    GLint view;
    GLint projection;
    GLint objectColour;
    GLint useTexture;
    GLint diffuseTexture;
    GLint emissiveStrength;
    GLint specularStrength;
    GLint shininess;
    GLint viewPosition;
    GLint ambientColour;
    std::array<GLint, 3> lightPositions;
    std::array<GLint, 3> lightColours;
    std::array<GLint, 3> lightIntensities;

    bool IsValid() const
    {
        if (model < 0 || view < 0 || projection < 0 || objectColour < 0 ||
            useTexture < 0 || diffuseTexture < 0 || emissiveStrength < 0 ||
            specularStrength < 0 || shininess < 0 || viewPosition < 0 ||
            ambientColour < 0)
            return false;
        for (int i = 0; i < 3; ++i)
        {
            if (lightPositions[i] < 0 || lightColours[i] < 0 || lightIntensities[i] < 0)
                return false;
        }
        return true;
    }
};

struct SceneAssets
{
    Model person;
    Model desk;
    Model monitor;
    Model keyboard;
    Model mouse;
    Texture errorScreen;

    bool Load(const std::filesystem::path& assetRoot)
    {
        const std::filesystem::path models = assetRoot / "Models";
        return person.Load(models / "person_angry" / "person_angry.obj") &&
               desk.Load(models / "desk_simple" / "desk_simple.obj") &&
               monitor.Load(models / "monitor" / "monitor.obj") &&
               keyboard.Load(models / "keyboard" / "keyboard.obj") &&
               mouse.Load(models / "mouse" / "mouse.obj") &&
               errorScreen.Load(assetRoot / "Textures" / "screen_error.png");
    }
};

const SceneTransform personTransform{{0.0f, 0.0f, -0.60f}, {1.0f, 1.0f, 1.0f}, 0.0f};
const SceneTransform deskTransform{{0.0f, 0.0f, 0.20f}, {1.12f, 1.0f, 1.0f}, 0.0f};
const std::array<SceneTransform, 3> monitorTransforms{{
    {{-0.62f, 0.752f, 0.45f}, {0.95f, 0.95f, 0.95f}, 165.0f},
    {{ 0.00f, 0.752f, 0.45f}, {1.08f, 1.08f, 1.08f}, 180.0f},
    {{ 0.62f, 0.752f, 0.45f}, {0.95f, 0.95f, 0.95f}, 195.0f}
}};
const SceneTransform keyboardTransform{{0.20f, 0.754f, -0.15f}, {1.45f, 1.45f, 1.45f}, 180.0f};
const SceneTransform mouseTransform{{0.55f, 0.754f, -0.14f}, {1.0f, 1.0f, 1.0f}, 90.0f};

const glm::vec3 cameraPosition(0.80f, 2.00f, -2.45f);
const glm::vec3 cameraTarget(0.0f, 0.95f, 0.38f);
constexpr float cameraFovDegrees = 46.0f;

const Material texturedMaterial{{1.0f, 1.0f, 1.0f}, true, 0.0f, 0.12f, 28.0f};

ShaderUniforms GetUniforms(const Shader& shader)
{
    ShaderUniforms uniforms{
        shader.GetUniformLocation("model"),
        shader.GetUniformLocation("view"),
        shader.GetUniformLocation("projection"),
        shader.GetUniformLocation("objectColour"),
        shader.GetUniformLocation("useTexture"),
        shader.GetUniformLocation("diffuseTexture"),
        shader.GetUniformLocation("emissiveStrength"),
        shader.GetUniformLocation("specularStrength"),
        shader.GetUniformLocation("shininess"),
        shader.GetUniformLocation("viewPosition"),
        shader.GetUniformLocation("ambientColour"),
        {}, {}, {}
    };
    for (int i = 0; i < 3; ++i)
    {
        const std::string suffix = "[" + std::to_string(i) + "]";
        uniforms.lightPositions[i] = shader.GetUniformLocation(("lightPositions" + suffix).c_str());
        uniforms.lightColours[i] = shader.GetUniformLocation(("lightColours" + suffix).c_str());
        uniforms.lightIntensities[i] = shader.GetUniformLocation(("lightIntensities" + suffix).c_str());
    }
    return uniforms;
}

glm::mat4 TransformMatrix(const SceneTransform& transform)
{
    glm::mat4 model(1.0f);
    model = glm::translate(model, transform.position);
    model = glm::rotate(model, glm::radians(transform.yawDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
    return glm::scale(model, transform.scale);
}

void SetMaterial(const ShaderUniforms& uniforms, const Material& material)
{
    glUniform3fv(uniforms.objectColour, 1, glm::value_ptr(material.colour));
    glUniform1i(uniforms.useTexture, material.useTexture ? GL_TRUE : GL_FALSE);
    glUniform1f(uniforms.emissiveStrength, material.emissive);
    glUniform1f(uniforms.specularStrength, material.specular);
    glUniform1f(uniforms.shininess, material.shininess);
}

void DrawMesh(const Mesh& mesh, const glm::mat4& model, const Material& material,
              const ShaderUniforms& uniforms)
{
    glUniformMatrix4fv(uniforms.model, 1, GL_FALSE, glm::value_ptr(model));
    SetMaterial(uniforms, material);
    mesh.RenderMesh();
}

void DrawModel(const Model& model, const SceneTransform& transform,
               const ShaderUniforms& uniforms)
{
    glUniformMatrix4fv(uniforms.model, 1, GL_FALSE,
                       glm::value_ptr(TransformMatrix(transform)));
    SetMaterial(uniforms, texturedMaterial);
    model.Render();
}

bool CreateCube(Mesh& cube)
{
    const std::vector<Vertex> vertices{
        // Back
        {-.5f,-.5f,-.5f, 0,0, 0,0,-1}, { .5f,-.5f,-.5f, 1,0, 0,0,-1},
        { .5f, .5f,-.5f, 1,1, 0,0,-1}, {-.5f, .5f,-.5f, 0,1, 0,0,-1},
        // Front
        {-.5f,-.5f, .5f, 0,0, 0,0, 1}, { .5f,-.5f, .5f, 1,0, 0,0, 1},
        { .5f, .5f, .5f, 1,1, 0,0, 1}, {-.5f, .5f, .5f, 0,1, 0,0, 1},
        // Left
        {-.5f,-.5f,-.5f, 0,0,-1,0,0}, {-.5f,-.5f, .5f, 1,0,-1,0,0},
        {-.5f, .5f, .5f, 1,1,-1,0,0}, {-.5f, .5f,-.5f, 0,1,-1,0,0},
        // Right
        { .5f,-.5f,-.5f, 0,0, 1,0,0}, { .5f, .5f,-.5f, 0,1, 1,0,0},
        { .5f, .5f, .5f, 1,1, 1,0,0}, { .5f,-.5f, .5f, 1,0, 1,0,0},
        // Bottom
        {-.5f,-.5f,-.5f, 0,0, 0,-1,0}, { .5f,-.5f,-.5f, 1,0, 0,-1,0},
        { .5f,-.5f, .5f, 1,1, 0,-1,0}, {-.5f,-.5f, .5f, 0,1, 0,-1,0},
        // Top
        {-.5f, .5f,-.5f, 0,0, 0,1,0}, {-.5f, .5f, .5f, 0,1, 0,1,0},
        { .5f, .5f, .5f, 1,1, 0,1,0}, { .5f, .5f,-.5f, 1,0, 0,1,0}
    };
    const std::vector<unsigned int> indices{
         0, 2, 1,  0, 3, 2,  4, 5, 6,  4, 6, 7,
         8, 9,10,  8,10,11, 12,13,14, 12,14,15,
        16,17,18, 16,18,19, 20,21,22, 20,22,23
    };
    return cube.CreateMesh(vertices, indices);
}

bool CreateQuad(Mesh& quad)
{
    const std::vector<Vertex> vertices{
        {-.5f,-.5f,0, 0,0, 0,0,1},
        { .5f,-.5f,0, 1,0, 0,0,1},
        { .5f, .5f,0, 1,1, 0,0,1},
        {-.5f, .5f,0, 0,1, 0,0,1}
    };
    const std::vector<unsigned int> indices{0,1,2, 0,2,3};
    return quad.CreateMesh(vertices, indices);
}

glm::mat4 BoxMatrix(const glm::vec3& position, const glm::vec3& size)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    return glm::scale(model, size);
}

void DrawRoom(const Mesh& cube, const ShaderUniforms& uniforms)
{
    auto box = [&](const glm::vec3& position, const glm::vec3& size,
                   const glm::vec3& colour, float emissive = 0.0f) {
        DrawMesh(cube, BoxMatrix(position, size),
                 {colour, false, emissive, 0.04f, 12.0f}, uniforms);
    };

    const glm::vec3 wall(0.18f, 0.16f, 0.17f);
    const glm::vec3 frame(0.055f, 0.065f, 0.085f);

    box({0.0f, -0.06f, -0.10f}, {6.4f, 0.12f, 6.4f}, {0.105f, 0.072f, 0.052f});
    box({0.0f, 2.85f, -0.10f}, {6.4f, 0.10f, 6.4f}, {0.105f, 0.095f, 0.105f});
    box({-3.15f, 1.40f, -0.10f}, {0.10f, 2.8f, 6.4f}, wall);
    box({ 3.15f, 1.40f, -0.10f}, {0.10f, 2.8f, 6.4f}, wall);

    // Back wall sections leave a large window opening behind the monitors.
    box({0.0f, 0.28f, 2.95f}, {6.4f, 0.56f, 0.12f}, wall);
    box({0.0f, 2.69f, 2.95f}, {6.4f, 0.32f, 0.12f}, wall);
    box({-2.68f, 1.49f, 2.95f}, {0.96f, 1.90f, 0.12f}, wall);
    box({ 2.68f, 1.49f, 2.95f}, {0.96f, 1.90f, 0.12f}, wall);

    box({0.0f, 1.50f, 3.04f}, {4.36f, 1.86f, 0.025f}, {0.012f, 0.028f, 0.065f}, 0.05f);

    struct Building { float x, width, height; glm::vec3 colour; };
    const std::array<Building, 7> buildings{{
        {-1.88f, 0.42f, 0.95f, {0.025f,0.035f,0.055f}},
        {-1.36f, 0.52f, 1.42f, {0.035f,0.045f,0.070f}},
        {-0.76f, 0.45f, 1.10f, {0.020f,0.032f,0.060f}},
        {-0.18f, 0.60f, 1.65f, {0.030f,0.040f,0.072f}},
        { 0.53f, 0.50f, 1.25f, {0.022f,0.037f,0.067f}},
        { 1.16f, 0.58f, 1.55f, {0.030f,0.043f,0.075f}},
        { 1.84f, 0.38f, 0.88f, {0.020f,0.032f,0.052f}}
    }};
    for (std::size_t buildingIndex = 0; buildingIndex < buildings.size(); ++buildingIndex)
    {
        const Building& building = buildings[buildingIndex];
        box({building.x, 0.57f + building.height * 0.5f, 3.005f},
            {building.width, building.height, 0.035f}, building.colour);

        const int columns = building.width > 0.5f ? 3 : 2;
        const int rows = static_cast<int>(building.height / 0.22f);
        for (int row = 0; row < rows; ++row)
        {
            for (int column = 0; column < columns; ++column)
            {
                if ((row + column + static_cast<int>(buildingIndex)) % 3 == 0)
                    continue;
                const float x = building.x +
                    (column - (columns - 1) * 0.5f) * building.width / columns * 0.72f;
                const float y = 0.68f + row * 0.20f;
                const glm::vec3 lightColour = (row + column) % 4 == 0
                    ? glm::vec3(0.28f, 0.52f, 0.80f)
                    : glm::vec3(0.95f, 0.62f, 0.24f);
                box({x, y, 2.978f}, {0.055f, 0.075f, 0.012f}, lightColour, 1.25f);
            }
        }
    }

    // Window surround and mullions sit in front of the city planes.
    for (float x : {-2.22f, -0.74f, 0.74f, 2.22f})
        box({x, 1.50f, 2.88f}, {0.075f, 1.94f, 0.075f}, frame);
    for (float y : {0.55f, 1.50f, 2.46f})
        box({0.0f, y, 2.88f}, {4.50f, 0.075f, 0.075f}, frame);

    // A small warm wall fixture gives the second light a visible source.
    box({-2.98f, 2.16f, 0.85f}, {0.10f, 0.24f, 0.42f}, {1.0f, 0.43f, 0.16f}, 1.8f);
}

void DrawMonitorScreens(const Mesh& quad, const SceneAssets& assets,
                        const ShaderUniforms& uniforms)
{
    const Material sideScreen{{0.018f, 0.045f, 0.085f}, false, 0.34f, 0.0f, 8.0f};
    const Material codeBlue{{0.12f, 0.55f, 0.90f}, false, 0.75f, 0.0f, 8.0f};
    const Material codeViolet{{0.53f, 0.28f, 0.92f}, false, 0.70f, 0.0f, 8.0f};

    for (int monitorIndex : {0, 2})
    {
        const glm::mat4 parent = TransformMatrix(monitorTransforms[monitorIndex]);
        glm::mat4 screen = glm::translate(parent, {0.0f, 0.32f, 0.036f});
        screen = glm::scale(screen, {0.59f, 0.332f, 1.0f});
        DrawMesh(quad, screen, sideScreen, uniforms);

        for (int line = 0; line < 6; ++line)
        {
            const float width = 0.18f + 0.045f * static_cast<float>((line + monitorIndex) % 4);
            const float x = -0.23f + width * 0.5f;
            const float y = 0.425f - line * 0.043f;
            glm::mat4 bar = glm::translate(parent, {x, y, 0.038f});
            bar = glm::scale(bar, {width, 0.012f, 1.0f});
            DrawMesh(quad, bar, line % 3 == 0 ? codeViolet : codeBlue, uniforms);
        }
    }

    const glm::mat4 centreParent = TransformMatrix(monitorTransforms[1]);
    glm::mat4 centreScreen = glm::translate(centreParent, {0.0f, 0.32f, 0.036f});
    centreScreen = glm::scale(centreScreen, {0.59f, 0.332f, 1.0f});
    assets.errorScreen.Bind();
    DrawMesh(quad, centreScreen,
             {{1.0f, 1.0f, 1.0f}, true, 0.72f, 0.0f, 8.0f}, uniforms);
}

void SetLights(const ShaderUniforms& uniforms)
{
    const std::array<glm::vec3, 3> positions{{
        {0.0f, 1.14f, 0.02f},
        {-2.55f, 2.28f, 0.72f},
        {1.75f, 1.62f, -1.25f}
    }};
    const std::array<glm::vec3, 3> colours{{
        {0.24f, 0.48f, 1.0f},
        {1.0f, 0.42f, 0.16f},
        {0.16f, 0.28f, 0.62f}
    }};
    const std::array<float, 3> intensities{{2.65f, 2.85f, 1.90f}};

    glUniform3fv(uniforms.viewPosition, 1, glm::value_ptr(cameraPosition));
    glUniform3f(uniforms.ambientColour, 0.045f, 0.050f, 0.075f);
    for (int i = 0; i < 3; ++i)
    {
        glUniform3fv(uniforms.lightPositions[i], 1, glm::value_ptr(positions[i]));
        glUniform3fv(uniforms.lightColours[i], 1, glm::value_ptr(colours[i]));
        glUniform1f(uniforms.lightIntensities[i], intensities[i]);
    }
}

void RenderScene(const SceneAssets& assets, const Mesh& cube, const Mesh& quad,
                 const ShaderUniforms& uniforms)
{
    DrawRoom(cube, uniforms);
    DrawModel(assets.person, personTransform, uniforms);
    DrawModel(assets.desk, deskTransform, uniforms);
    for (const SceneTransform& monitor : monitorTransforms)
        DrawModel(assets.monitor, monitor, uniforms);
    DrawModel(assets.keyboard, keyboardTransform, uniforms);
    DrawModel(assets.mouse, mouseTransform, uniforms);
    DrawMonitorScreens(quad, assets, uniforms);
}

// Actual framebuffer readback for visual verification; PPM is not the final submission.
bool Capture(const std::filesystem::path& path, int width, int height)
{
    std::vector<unsigned char> pixels(static_cast<std::size_t>(width) * height * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    std::ofstream output(path, std::ios::binary);
    output << "P6\n" << width << ' ' << height << "\n255\n";
    for (int y = height - 1; y >= 0; --y)
        output.write(reinterpret_cast<const char*>(pixels.data() +
                     static_cast<std::size_t>(y) * width * 3), width * 3);
    return output.good();
}
}

int main(int argc, char** argv)
{
    const bool capture = argc == 3 && std::string(argv[1]) == "--capture";
    Window window(1280, 720, 3, 3);
    if (window.initialise() != 0)
        return 1;
    glfwSetWindowTitle(window.getWindow(),
                       "Assignment 2 - Programmer at Night | Rear View | Esc: exit");
    if (capture)
        glfwHideWindow(window.getWindow());

    // GPU resources are destroyed before the window and its OpenGL context.
    Mesh cube;
    Mesh quad;
    if (!CreateCube(cube) || !CreateQuad(quad))
        return 1;

    SceneAssets assets;
    if (!assets.Load(std::filesystem::u8path(OPENGL_STARTER_ASSET_DIR)))
        return 1;

    Shader shader;
    const std::filesystem::path shaders = std::filesystem::u8path(OPENGL_STARTER_SHADER_DIR);
    shader.CreateFromFiles(shaders / "shader.vert", shaders / "shader.frag");
    shader.UseShader();
    const ShaderUniforms uniforms = GetUniforms(shader);
    if (!uniforms.IsValid())
    {
        std::cerr << "One or more required shader uniforms were not found.\n";
        return 1;
    }
    glUniform1i(uniforms.diffuseTexture, 0);

    const glm::mat4 camera = glm::lookAt(cameraPosition, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    while (!window.getShouldClose())
    {
        glfwPollEvents();
        if (glfwGetKey(window.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window.getWindow(), GLFW_TRUE);

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window.getWindow(), &width, &height);
        if (width <= 0 || height <= 0)
        {
            glfwWaitEvents();
            continue;
        }

        glViewport(0, 0, width, height);
        glClearColor(0.008f, 0.012f, 0.026f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.UseShader();

        const glm::mat4 perspective = glm::perspective(glm::radians(cameraFovDegrees),
            static_cast<float>(width) / static_cast<float>(height), 0.08f, 100.0f);
        glUniformMatrix4fv(uniforms.view, 1, GL_FALSE, glm::value_ptr(camera));
        glUniformMatrix4fv(uniforms.projection, 1, GL_FALSE, glm::value_ptr(perspective));
        SetLights(uniforms);
        RenderScene(assets, cube, quad, uniforms);

        if (capture)
        {
            if (!Capture(std::filesystem::u8path(argv[2]), width, height))
                return 1;
            const GLenum error = glGetError();
            std::cout << "Captured " << width << 'x' << height
                      << "; GL error: " << error << '\n';
            return error == GL_NO_ERROR ? 0 : 1;
        }
        window.swapBuffers();
    }
    return 0;
}
