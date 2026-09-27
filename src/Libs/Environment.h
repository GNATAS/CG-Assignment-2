#pragma once
#include "Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <vector>

// Static environment geometry is combined by material at startup, avoiding
// thousands of per-window draw calls. Exterior ground is 18 m below the room.
class Environment
{
public:
    struct Part { Mesh mesh; glm::vec3 colour; float emissive; float specular=.025f; float shininess=16.0f; };
    struct TowerPlacement { int kind; glm::vec3 position; glm::vec3 scale; float yaw; };
    static std::array<TowerPlacement,8> Towers()
    {
        return {{{0,{-12.0f,-18.0f,40.0f},{18.0f,20.0f,20.0f},12.0f},
                 {1,{-3.0f,-18.0f,35.0f},{13.0f,16.0f,15.0f},-16.0f},
                 {2,{6.0f,-18.0f,42.0f},{13.0f,19.0f,13.0f},18.0f},
                 {0,{15.0f,-18.0f,48.0f},{18.0f,25.0f,18.0f},-15.0f},
                 {1,{-22.0f,-18.0f,50.0f},{17.0f,20.0f,17.0f},20.0f},
                 {2,{-9.0f,-18.0f,57.0f},{16.0f,23.0f,16.0f},-10.0f},
                 {0,{4.0f,-18.0f,62.0f},{22.0f,27.0f,22.0f},16.0f},
                 {1,{23.0f,-18.0f,64.0f},{16.0f,21.0f,16.0f},-22.0f}}};
    }
    std::vector<Part> parts;

    bool Build()
    {
        const std::array<glm::vec3,21> colours{{
            {.48f,.46f,.42f}, {.26f,.16f,.085f}, {.10f,.085f,.065f},
            {.065f,.075f,.085f}, {.60f,.57f,.50f}, {.017f,.030f,.065f},
            {.075f,.095f,.13f}, {.10f,.12f,.15f}, {.042f,.065f,.11f},
            {.72f,.52f,.29f}, {.26f,.45f,.65f}, {.075f,.13f,.20f},
            {.34f,.34f,.32f}, {.8f,.15f,.08f},
            {.60f,.20f,.15f}, {.12f,.32f,.36f}, {.55f,.40f,.16f},
            {.80f,.77f,.66f}, {.21f,.31f,.40f}, {.34f,.43f,.50f}, {.42f,.45f,.49f}
        }};
        const std::array<float,21> emission{{.025f,0,0,0,.02f,1,.40f,.40f,.65f,1.1f,.85f,.45f,.1f,1,
            .10f,.10f,.10f,.06f,.05f,.05f,0}};
        vertices.resize(colours.size()); indices.resize(colours.size());
        // Room shell, with a real opening on the far wall.
        Box(0,{0,2.85f,-.1f},{6.4f,.12f,6.4f});
        Box(0,{-3.16f,1.4f,-.1f},{.12f,2.8f,6.4f});
        Box(0,{3.16f,1.4f,-.1f},{.12f,2.8f,6.4f});
        // Smaller window: 3.30 m wide, sill at .98 m and lintel at 2.36 m.
        // Leave a solid wall above the bed for art and a small display shelf.
        Box(0,{0,.49f,2.96f},{6.4f,.98f,.14f});
        Box(0,{0,2.58f,2.96f},{6.4f,.44f,.14f});
        Box(0,{-2.275f,1.67f,2.96f},{1.85f,1.38f,.14f});
        Box(0,{2.575f,1.67f,2.96f},{1.25f,1.38f,.14f});
        // Separate wood planks with narrow recessed seams.
        Box(2,{0,-.045f,-.1f},{6.4f,.08f,6.4f});
        for (int col=0;col<16;++col)
            for (int row=0;row<5;++row)
                Box(1,{-3.0f+col*.40f,-.006f,-2.66f+row*1.28f},
                    {.392f,.015f,1.27f});
        Box(4,{0,.07f,2.85f},{6.2f,.14f,.06f});
        for (float x : {-3.075f,3.075f})
            Box(4,{x,.07f,-.1f},{.045f,.14f,6.3f});
        Box(4,{.30f,.97f,2.80f},{3.46f,.075f,.32f});
        for (float x : {-1.35f,-.25f,.85f,1.95f})
            Box(3,{x,1.67f,2.84f},{.055f,1.43f,.075f});
        for (float y : {.98f,2.36f})
            Box(3,{.30f,y,2.84f},{3.36f,.055f,.075f});
        // Small window handle, trim and electrical outlet.
        Box(12,{-.19f,1.52f,2.775f},{.025f,.14f,.035f});
        Box(4,{-2.90f,.30f,2.87f},{.14f,.19f,.035f});
        for (float x : {-2.927f,-2.873f})
            Box(3,{x,.30f,2.847f},{.012f,.035f,.008f});

        // Metal curtain pole, capped ends and three brackets anchored to the wall.
        Cylinder(20,{-1.86f,2.47f,2.59f},{2.40f,2.47f,2.59f},.023f);
        for(float x : {-1.86f,2.40f})
            Cylinder(20,{x-.028f,2.47f,2.59f},{x+.028f,2.47f,2.59f},.045f);
        for(float x : {-1.66f,.27f,2.20f})
        {
            Cylinder(20,{x,2.40f,2.87f},{x,2.40f,2.895f},.052f);
            Cylinder(20,{x,2.40f,2.87f},{x,2.40f,2.59f},.013f);
            Cylinder(20,{x,2.40f,2.59f},{x,2.47f,2.59f},.013f);
        }

        // Fabric rug and border below the workstation, clear of the floor.
        Box(18,{.25f,.009f,-.65f},{2.40f,.012f,2.50f});
        for(float x : {-.90f,1.40f})
            Box(19,{x,.017f,-.65f},{.032f,.004f,2.40f});
        for(float z : {-1.85f,.55f})
            Box(19,{.25f,.017f,z},{2.30f,.004f,.032f});

        Box(5,{0,10,140.0f},{240,140,.2f});
        // Exterior buildings and window illumination are rendered on OBJ meshes.
        for (size_t i=0;i<colours.size();++i)
        {
            if(indices[i].empty()) continue;
            Part part;
            part.colour=colours[i]; part.emissive=emission[i];
            if(i==20) { part.specular=.65f; part.shininess=64.0f; }
            if(!part.mesh.CreateMesh(vertices[i],indices[i]))
            {
                std::cerr << "Environment mesh failed for material " << i << '\n';
                return false;
            }
            parts.push_back(std::move(part));
        }
        vertices.clear(); indices.clear();
        return true;
    }
private:
    std::vector<std::vector<Vertex>> vertices;
    std::vector<std::vector<unsigned int>> indices;
    void Cylinder(int material,glm::vec3 a,glm::vec3 b,float radius)
    {
        const glm::vec3 axis=glm::normalize(b-a);
        const glm::vec3 helper=std::abs(axis.y)<.9f?glm::vec3(0,1,0):glm::vec3(1,0,0);
        const glm::vec3 u=glm::normalize(glm::cross(axis,helper));
        const glm::vec3 v=glm::cross(axis,u);
        auto& vs=vertices[material]; auto& is=indices[material];
        auto vertex=[&](glm::vec3 p,glm::vec3 n) {
            vs.push_back({p.x,p.y,p.z,0,0,n.x,n.y,n.z});
        };
        constexpr int segments=24;
        for(int i=0;i<segments;++i)
        {
            const float t0=6.283185307f*i/segments, t1=6.283185307f*(i+1)/segments;
            const glm::vec3 n0=u*std::cos(t0)+v*std::sin(t0);
            const glm::vec3 n1=u*std::cos(t1)+v*std::sin(t1);
            unsigned int start=static_cast<unsigned int>(vs.size());
            vertex(a+radius*n0,n0); vertex(a+radius*n1,n1);
            vertex(b+radius*n1,n1); vertex(b+radius*n0,n0);
            for(unsigned int k : {0u,1u,2u,0u,2u,3u}) is.push_back(start+k);
            start=static_cast<unsigned int>(vs.size());
            vertex(a,-axis); vertex(a+radius*n1,-axis); vertex(a+radius*n0,-axis);
            vertex(b,axis); vertex(b+radius*n0,axis); vertex(b+radius*n1,axis);
            for(unsigned int k=0;k<6;++k) is.push_back(start+k);
        }
    }
    void Box(int material,glm::vec3 centre,glm::vec3 size,
             const glm::mat4& parent=glm::mat4(1))
    {
        static const glm::vec3 corners[8]={{-.5f,-.5f,-.5f},{.5f,-.5f,-.5f},
            {.5f,.5f,-.5f},{-.5f,.5f,-.5f},{-.5f,-.5f,.5f},{.5f,-.5f,.5f},
            {.5f,.5f,.5f},{-.5f,.5f,.5f}};
        static const int faces[6][4]={{0,3,2,1},{4,5,6,7},{0,4,7,3},
            {1,2,6,5},{0,1,5,4},{3,7,6,2}};
        static const glm::vec3 normals[6]={{0,0,-1},{0,0,1},{-1,0,0},{1,0,0},{0,-1,0},{0,1,0}};
        auto& vs=vertices[material]; auto& is=indices[material];
        for(int f=0;f<6;++f)
        {
            unsigned int start=static_cast<unsigned int>(vs.size());
            const glm::vec3 n=glm::mat3(parent)*normals[f];
            for(int j=0;j<4;++j)
            {
                const glm::vec3 p=glm::vec3(parent*glm::vec4(centre+corners[faces[f][j]]*size,1));
                vs.push_back({p.x,p.y,p.z,0,0,n.x,n.y,n.z});
            }
            for(unsigned int k : {0u,1u,2u,0u,2u,3u}) is.push_back(start+k);
        }
    }
};
