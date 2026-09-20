#undef GLFW_DLL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Libs/Mesh.h"
#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "ProjectPaths.h"

// Empty room blockout: Y up, back wall Z=-3, window wall X=-4.
// Units approximate metres. Place future furniture inside the room.
void CreateCube(Mesh& cube)
{
    GLfloat vertices[] = {
        -.5f,-.5f,-.5f, .5f,-.5f,-.5f, .5f,.5f,-.5f, -.5f,.5f,-.5f,
        -.5f,-.5f,.5f, .5f,-.5f,.5f, .5f,.5f,.5f, -.5f,.5f,.5f
    };
    unsigned int indices[] = {
        0,2,1,0,3,2, 4,5,6,4,6,7, 0,4,7,0,7,3,
        1,2,6,1,6,5, 3,7,6,3,6,2, 0,1,5,0,5,4
    };
    cube.CreateMesh(vertices,indices,24,36);
}

void DrawRoom(Mesh& cube, GLint modelLocation, GLint colourLocation)
{
    auto box = [&](glm::vec3 position, glm::vec3 size, glm::vec3 colour) {
        glm::mat4 model = glm::translate(glm::mat4(1),position);
        model = glm::scale(model,size);
        glUniformMatrix4fv(modelLocation,1,GL_FALSE,glm::value_ptr(model));
        glUniform3fv(colourLocation,1,glm::value_ptr(colour));
        cube.RenderMesh();
    };
    const glm::vec3 wall(.55f,.57f,.59f), trim(.77f,.78f,.76f), frame(.075f,.095f,.115f);
    // Keep the front open; the camera sits inside the room.
    box({0,-.1f,1},{8,.2f,8},{.39f,.32f,.26f});
    box({0,3.7f,1},{8,.2f,8},{.34f,.36f,.39f});
    box({0,1.8f,-3.1f},{8,3.6f,.2f},wall);
    box({4.1f,1.8f,1},{.2f,3.6f,8},{.47f,.49f,.52f});
    // Wall sections around the left window opening.
    box({-4.1f,.2f,1},{.2f,.4f,8},wall);
    box({-4.1f,3.45f,1},{.2f,.3f,8},wall);
    box({-4.1f,1.85f,-2.85f},{.2f,2.9f,.3f},wall);
    box({-4.1f,1.85f,4.25f},{.2f,2.9f,1.5f},wall);
    // Flat night backdrop: replace with a city texture/scenery later.
    box({-4.23f,1.85f,.4f},{.04f,2.9f,6.2f},{.055f,.105f,.16f});
    for (float z : {-2.7f,-.63f,1.43f,3.5f})
        box({-3.96f,1.85f,z},{.16f,2.9f,.09f},frame);
    for (float y : {.43f,3.27f})
        box({-3.96f,y,.4f},{.16f,.10f,6.3f},frame);
    box({-3.88f,.36f,.4f},{.4f,.1f,6.45f},trim);
    // Skirting and floorboard seams (geometry, not lighting/shadows).
    box({0,.09f,-2.97f},{8,.18f,.07f},trim);
    box({3.97f,.09f,1},{.07f,.18f,8},trim);
    box({-3.97f,.09f,1},{.07f,.18f,8},trim);
    for (int i=1;i<16;++i)
        box({-4.f+i*.5f,.002f,1},{.014f,.004f,8},{.28f,.23f,.19f});
}

// Actual framebuffer readback for visual verification; PPM is not the final submission.
bool Capture(const std::filesystem::path& path,int width,int height)
{
    std::vector<unsigned char> pixels(size_t(width)*height*3);
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadBuffer(GL_BACK);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    std::ofstream output(path,std::ios::binary);
    output << "P6\n" << width << ' ' << height << "\n255\n";
    for (int y=height-1;y>=0;--y)
        output.write(reinterpret_cast<const char*>(pixels.data()+size_t(y)*width*3),width*3);
    return output.good();
}

int main(int argc,char** argv)
{
    const bool capture=argc==3 && std::string(argv[1])=="--capture";
    Window window(800,600,3,3);
    if (window.initialise()!=0) return 1;
    glfwSetWindowTitle(window.getWindow(),"Assignment 2 - Future Me | Empty Room | Esc: exit");
    if (capture) glfwHideWindow(window.getWindow());
    // Resources are destroyed before the window and its GL context.
    Mesh cube;
    CreateCube(cube);
    Shader shader;
    const auto shaders=std::filesystem::u8path(OPENGL_STARTER_SHADER_DIR);
    shader.CreateFromFiles(shaders/"shader.vert",shaders/"shader.frag");
    const GLint model=shader.GetUniformLocation("model");
    const GLint view=shader.GetUniformLocation("view");
    const GLint projection=shader.GetUniformLocation("projection");
    const GLint colour=shader.GetUniformLocation("objectColour");
    if (model<0 || view<0 || projection<0 || colour<0) return 1;
    // Looking from the front-right towards the window/back wall.
    const glm::mat4 camera=glm::lookAt(glm::vec3(3.1f,2.4f,4.7f),
        glm::vec3(-.8f,1.6f,-1.4f),glm::vec3(0,1,0));
    while (!window.getShouldClose())
    {
        glfwPollEvents();
        if (glfwGetKey(window.getWindow(),GLFW_KEY_ESCAPE)==GLFW_PRESS)
            glfwSetWindowShouldClose(window.getWindow(),GLFW_TRUE);
        int width,height;
        glfwGetFramebufferSize(window.getWindow(),&width,&height);
        if (width<=0 || height<=0) { glfwWaitEvents(); continue; }
        glViewport(0,0,width,height);
        glClearColor(.055f,.075f,.10f,1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        shader.UseShader();
        const glm::mat4 perspective=glm::perspective(glm::radians(62.f),
            float(width)/height,.1f,100.f);
        glUniformMatrix4fv(view,1,GL_FALSE,glm::value_ptr(camera));
        glUniformMatrix4fv(projection,1,GL_FALSE,glm::value_ptr(perspective));
        DrawRoom(cube,model,colour);
        if (capture)
        {
            if (!Capture(std::filesystem::u8path(argv[2]),width,height)) return 1;
            const GLenum error=glGetError();
            std::cout << "Captured " << width << 'x' << height << "; GL error: " << error << '\n';
            return error==GL_NO_ERROR ? 0 : 1;
        }
        window.swapBuffers();
    }
    return 0;
}
