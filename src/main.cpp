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
#include "Libs/Environment.h"
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
    bool cityLighting = false;
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
    GLint cityLighting;
    std::array<GLint, 4> lightPositions;
    std::array<GLint, 4> lightColours;
    std::array<GLint, 4> lightIntensities;

    bool IsValid() const
    {
        if (model < 0 || view < 0 || projection < 0 || objectColour < 0 ||
            useTexture < 0 || diffuseTexture < 0 || emissiveStrength < 0 ||
            specularStrength < 0 || shininess < 0 || viewPosition < 0 ||
            ambientColour < 0 || cityLighting < 0)
            return false;
        for (int i = 0; i < 4; ++i)
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
    Model plant;
    Model lamp;
    Model bed;
    Model towerA;
    Model towerB;
    Model towerC;
    Model curtains;
    Model wallBookshelf;
    Model books;
    Model sideTable;
    Model wallClock;
    Texture errorScreen;

    bool Load(const std::filesystem::path& assetRoot)
    {
        const std::filesystem::path models = assetRoot / "Models";
        return person.Load(models / "person_angry" / "person_angry.obj") &&
               desk.Load(models / "desk_simple" / "desk_simple.obj") &&
               monitor.Load(models / "monitor" / "monitor.obj") &&
               keyboard.Load(models / "keyboard" / "keyboard.obj") &&
               mouse.Load(models / "mouse" / "mouse.obj") &&
               plant.Load(models / "bk_plant" / "bk_plant.obj") &&
               lamp.Load(models / "bk_lamp" / "bk_lamp.obj") &&
               bed.Load(models / "bk_bed_alt" / "bk_bed_alt.obj") &&
               towerA.Load(models / "bk_city_glass" / "bk_city_glass.obj") &&
               towerB.Load(models / "bk_city_office" / "bk_city_office.obj") &&
               towerC.Load(models / "bk_city_highrise" / "bk_city_highrise.obj") &&
               curtains.Load(models / "curtain" / "curtain.obj") &&
               wallBookshelf.Load(models / "bk_wall_shelf" / "bk_wall_shelf.obj") &&
               books.Load(models / "bk_books" / "bk_books.obj") &&
               sideTable.Load(models / "bk_side_table" / "bk_side_table.obj") &&
               wallClock.Load(models / "bk_wall_clock" / "bk_wall_clock.obj") &&
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
constexpr float cameraFovDegrees = 52.0f;

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
        shader.GetUniformLocation("cityLighting"),
        {}, {}, {}
    };
    for (int i = 0; i < 4; ++i)
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
    glUniform1i(uniforms.cityLighting, material.cityLighting ? GL_TRUE : GL_FALSE);
}

void DrawMesh(const Mesh& mesh, const glm::mat4& model, const Material& material,
              const ShaderUniforms& uniforms)
{
    glUniformMatrix4fv(uniforms.model, 1, GL_FALSE, glm::value_ptr(model));
    SetMaterial(uniforms, material);
    mesh.RenderMesh();
}

void DrawModel(const Model& model, const SceneTransform& transform,
               const ShaderUniforms& uniforms, bool cityLighting = false)
{
    glUniformMatrix4fv(uniforms.model, 1, GL_FALSE,
                       glm::value_ptr(TransformMatrix(transform)));
    Material material = texturedMaterial;
    material.cityLighting = cityLighting;
    material.useTexture = model.HasTexture();
    if (!material.useTexture)
        material.colour = model.GetDiffuseColour();
    SetMaterial(uniforms, material);
    model.Render();
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


void DrawMonitorScreens(const Mesh& quad, const SceneAssets& assets,
                        const ShaderUniforms& uniforms)
{
    const Material sideScreen{{0.014f, 0.030f, 0.060f}, false, 0.24f, 0.0f, 8.0f};
    const Material codeBlue{{0.10f, 0.42f, 0.78f}, false, 0.60f, 0.0f, 8.0f};
    const Material codeViolet{{0.42f, 0.25f, 0.72f}, false, 0.56f, 0.0f, 8.0f};

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
             {{1.0f, 1.0f, 1.0f}, true, 0.82f, 0.0f, 8.0f}, uniforms);
}

void SetLights(const ShaderUniforms& uniforms)
{
    // All visible light now has a believable source: monitors, desk lamp, and city/window glow.
    const std::array<glm::vec3, 4> positions{{
        { 0.00f, 1.14f,  0.28f}, // monitor glow, just in front of the screens
        {-0.58f, 1.02f, -0.10f}, // warm desk lamp
        {-0.85f, 1.75f,  3.35f}, // cool city/sky glow through the window
        { 1.20f, 1.55f,  3.20f}  // softer warm city-window bounce
    }};
    const std::array<glm::vec3, 4> colours{{
        {0.14f, 0.29f, 0.72f},
        {1.00f, 0.43f, 0.20f},
        {0.18f, 0.30f, 0.56f},
        {0.72f, 0.42f, 0.20f}
    }};
    const std::array<float, 4> intensities{{2.55f, 0.48f, 1.45f, 0.55f}};

    glUniform3fv(uniforms.viewPosition, 1, glm::value_ptr(cameraPosition));
    glUniform3f(uniforms.ambientColour, 0.045f, 0.050f, 0.070f);
    for (int i = 0; i < 4; ++i)
    {
        glUniform3fv(uniforms.lightPositions[i], 1, glm::value_ptr(positions[i]));
        glUniform3fv(uniforms.lightColours[i], 1, glm::value_ptr(colours[i]));
        glUniform1f(uniforms.lightIntensities[i], intensities[i]);
    }
}

void RenderScene(const SceneAssets& assets, const Environment& environment, const Mesh& quad,
                 const ShaderUniforms& uniforms)
{
    for (const auto& part : environment.parts)
        DrawMesh(part.mesh, glm::mat4(1), {part.colour,false,part.emissive,part.specular,part.shininess}, uniforms);
    for(const auto& tower:Environment::Towers())
    {
        const Model& model=tower.kind==0?assets.towerA:
                           tower.kind==1?assets.towerB:assets.towerC;
        DrawModel(model,{tower.position,tower.scale,tower.yaw},uniforms,true);
    }
    for(int row=0;row<2;++row)
        for(int col=-4;col<=4;++col)
        {
            const int seed=(col+8)*37+row*83;
            const float width=10.0f+(seed%5);
            const float height=18.0f+(seed%14);
            const Model& model=seed%3==0?assets.towerA:seed%3==1?assets.towerB:assets.towerC;
            DrawModel(model,
                {{col*10.5f+row*3.0f,-18.0f,80.0f+row*20.0f},
                 {width,height,width},12.0f+(seed%4)*11.0f},uniforms,true);
        }
    DrawModel(assets.curtains, {{.27f,.79f,2.65f},{1.08f,.61f,.50f},180.0f}, uniforms);
    // Open-front wood bookshelf fixed to the wall above the bed's headboard.
    // The shelf's bottom deck is at local y=.04; books rest on that surface.
    // Right edge x=-2.107 leaves >.32 m to the curtain's nearest edge.
    DrawModel(assets.wallBookshelf, {{-2.51f,1.38f,2.63f},{.43f,.54f,.52f},180.0f}, uniforms);
    for(float x : {-2.77f,-2.54f})
        DrawModel(assets.books, {{x,1.402f,2.59f},{.25f,.25f,.25f},180.0f}, uniforms);
    DrawModel(assets.plant, {{-2.25f,1.402f,2.58f},{.26f,.26f,.26f},25.0f}, uniforms);
    DrawModel(assets.wallClock, {{-2.51f,2.05f,2.86f},{.36f,.36f,.36f},0.0f}, uniforms);
    // Side table beside the computer desk, with the lamp arm aimed at the sitter.
    DrawModel(assets.sideTable, {{1.30f,0.0f,-.10f},{.68f,.68f,.68f},0.0f}, uniforms);
    DrawModel(assets.lamp, {{1.31f,.68f,-.18f},{.39f,.39f,.39f},-110.0f}, uniforms);
    DrawModel(assets.plant, {{1.16f,.68f,.06f},{.23f,.23f,.23f},0.0f}, uniforms);
    DrawModel(assets.plant, {{2.04f,0.0f,2.18f},{1.32f,1.32f,1.32f},35.0f}, uniforms);
    DrawModel(assets.plant, {{-1.30f,0.0f,2.05f},{1.35f,1.35f,1.35f},0.0f}, uniforms);
    DrawModel(assets.bed, {{-2.15f,0.0f,0.65f},{.60f,.85f,.90f},180.0f}, uniforms);
    DrawModel(assets.person, personTransform, uniforms);
    DrawModel(assets.desk, deskTransform, uniforms);
    for (const SceneTransform& monitor : monitorTransforms)
        DrawModel(assets.monitor, monitor, uniforms);
    DrawModel(assets.keyboard, keyboardTransform, uniforms);
    DrawModel(assets.mouse, mouseTransform, uniforms);
    DrawModel(assets.lamp, {{-0.60f,0.754f,-0.12f},{.43f,.43f,.43f},180.0f}, uniforms);
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
    Window window(800, 600, 3, 3);
    if (window.initialise() != 0)
        return 1;
    glfwSetWindowTitle(window.getWindow(),
                       "Assignment 2 - Programmer at Night | Rear View | Esc: exit");
    if (capture)
        glfwHideWindow(window.getWindow());

    // GPU resources are destroyed before the window and its OpenGL context.
    Mesh quad;
    if (!CreateQuad(quad))
        return 1;
    Environment environment;
    if (!environment.Build())
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
            static_cast<float>(width) / static_cast<float>(height), 0.08f, 180.0f);
        glUniformMatrix4fv(uniforms.view, 1, GL_FALSE, glm::value_ptr(camera));
        glUniformMatrix4fv(uniforms.projection, 1, GL_FALSE, glm::value_ptr(perspective));
        SetLights(uniforms);
        RenderScene(assets, environment, quad, uniforms);

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
