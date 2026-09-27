#pragma once
#include "Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <vector>

// Static environment geometry is combined by material at startup, avoiding
// thousands of per-window draw calls. Exterior ground is 18 m below the room.
class Environment
{
public:
    struct Part { Mesh mesh; glm::vec3 colour; float emissive; };
    std::vector<Part> parts;

    bool Build()
    {
        const std::array<glm::vec3,20> colours{{
            {.48f,.46f,.42f}, {.26f,.16f,.085f}, {.10f,.085f,.065f},
            {.065f,.075f,.085f}, {.60f,.57f,.50f}, {.017f,.030f,.065f},
            {.075f,.095f,.13f}, {.10f,.12f,.15f}, {.042f,.065f,.11f},
            {.72f,.52f,.29f}, {.26f,.45f,.65f}, {.075f,.13f,.20f},
            {.34f,.34f,.32f}, {.8f,.15f,.08f},
            {.60f,.20f,.15f}, {.12f,.32f,.36f}, {.55f,.40f,.16f},
            {.80f,.77f,.66f}, {.21f,.31f,.40f}, {.34f,.43f,.50f}
        }};
        const std::array<float,20> emission{{.025f,0,0,0,.02f,1,.40f,.40f,.65f,1.1f,.85f,.45f,.1f,1,
            .10f,.10f,.10f,.06f,.05f,.05f}};
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

        // Gathered fabric curtains. Alternating depths describe the folds
        // without transparent surfaces or coplanar overlays.
        Box(3,{.30f,2.48f,2.60f},{3.94f,.035f,.035f});
        for(float side : {-1.55f,2.15f})
        {
            for(int fold=0;fold<7;++fold)
                Box(fold%2?18:19,{side+(fold-3)*.046f,1.64f,
                    2.61f-(fold%2)*.035f},{.052f,1.62f,.055f});
            Box(16,{side,1.34f,2.55f},{.34f,.045f,.028f});
        }

        // Original geometric wall print (no text or downloaded artwork).
        Box(2,{-2.34f,2.01f,2.82f},{.86f,.91f,.065f});
        Box(17,{-2.34f,2.01f,2.778f},{.78f,.83f,.012f});
        Box(18,{-2.34f,1.83f,2.766f},{.66f,.33f,.009f});
        Box(15,{-2.53f,1.98f,2.754f},{.24f,.35f,.009f});
        Box(16,{-2.17f,2.18f,2.754f},{.22f,.22f,.009f});
        Box(14,{-2.24f,1.93f,2.743f},{.26f,.09f,.009f});

        // Floating shelf above the headboard, with small books and a photo frame.
        Box(1,{-2.34f,1.36f,2.65f},{1.07f,.055f,.38f});
        for(int book=0;book<5;++book)
        {
            float h=.19f+(book%3)*.025f;
            Box(14+book%3,{-2.68f+book*.055f,1.39f+h*.5f,2.64f},
                {.045f,h,.17f});
        }
        Box(3,{-2.03f,1.52f,2.60f},{.24f,.26f,.035f});
        Box(17,{-2.03f,1.52f,2.578f},{.20f,.22f,.009f});
        Box(15,{-2.03f,1.485f,2.568f},{.17f,.12f,.009f});

        // Fabric rug and border below the workstation, clear of the floor.
        Box(18,{.25f,.009f,-.65f},{2.40f,.012f,2.50f});
        for(float x : {-.90f,1.40f})
            Box(19,{x,.017f,-.65f},{.032f,.004f,2.40f});
        for(float z : {-1.85f,.55f})
            Box(19,{.25f,.017f,z},{2.30f,.004f,.032f});

        // Books share the imported shelf's position/orientation. Shelf levels
        // were measured from its OBJ at the displayed 1.85 m height.
        const glm::mat4 shelf=glm::rotate(
            glm::translate(glm::mat4(1),{2.78f,0,1.05f}),
            glm::radians(-90.0f),{0,1,0});
        const std::array<float,6> levels{{.10f,.35f,.60f,.86f,1.14f,1.49f}};
        for(int row=0;row<6;++row)
        {
            for(int book=0;book<10;++book)
            {
                const float height=.16f+((row*3+book)%4)*.017f;
                const float x=-.47f+book*.067f;
                const int cover=14+(row+book)%3;
                Box(cover,{x,levels[row]+height*.5f,.075f},{.055f,height,.29f},shelf);
                Box(17,{x,levels[row]+height-.014f,.066f},{.041f,.01f,.25f},shelf);
                // Fine gold/cream spine bands rather than unreadable text.
                for(float offset : {.035f,.07f})
                    Box(17,{x,levels[row]+offset,.224f},{.044f,.006f,.006f},shelf);
            }
            for(int book=0;book<3;++book)
                Box(14+(row+book)%3,{.37f,levels[row]+.017f+book*.036f,.07f},
                    {.27f,.031f,.28f},shelf);
        }
        // Single bed along the opposite wall (screen-right in this rear view).
        Box(1,{-2.15f,.23f,.65f},{1.40f,.28f,2.35f});
        for(float x : {-2.71f,-1.59f})
            for(float z : {-.38f,1.68f})
                Box(2,{x,.10f,z},{.09f,.20f,.09f});
        Box(18,{-2.15f,.64f,1.88f},{1.46f,1.05f,.12f}); // upholstered headboard
        for(int panel=0;panel<5;++panel)
            Box(19,{-2.71f+panel*.28f,.73f,1.808f},{.26f,.74f,.024f});
        Box(17,{-2.15f,.43f,.65f},{1.36f,.20f,2.25f}); // mattress
        Box(18,{-2.15f,.555f,.23f},{1.38f,.065f,1.44f}); // duvet
        Box(19,{-2.15f,.598f,.88f},{1.39f,.04f,.24f}); // folded edge
        for(float x : {-2.78f,-1.52f})
            Box(18,{x,.46f,.23f},{.06f,.18f,1.45f}); // hanging sides
        Box(17,{-2.15f,.60f,1.38f},{.93f,.15f,.43f}); // pillow
        Box(4,{-2.15f,.676f,1.38f},{.80f,.014f,.34f});

        Box(5,{0,10,85.0f},{180,120,.2f});
        // A real 3D skyline: depth, rotated footprints, side windows and roofs.
        for (int row=0;row<4;++row)
        {
            const float z=22.0f+row*14.0f;
            for (int col=-6;col<=6;++col)
            {
                const int seed=(col+8)*37+row*83;
                const float x=col*(4.4f+row*.65f)+(row%2)*1.8f;
                const float width=2.2f+(seed%5)*.27f;
                const float depth=2.3f+(seed%3)*.5f;
                const float height=10.0f+(seed%14)*1.0f;
                const float yaw=12.0f+(seed%4)*7.0f;
                const glm::mat4 parent=glm::rotate(
                    glm::translate(glm::mat4(1),{x,-18.0f,z}),
                    glm::radians(yaw),{0,1,0});
                const int facade=row>=2?8:6+seed%2;
                Box(facade,{0,height*.5f,0},{width,height,depth},parent);
                Box(7,{0,height+.10f,0},{width+.12f,.20f,depth+.12f},parent);
                Box(12,{width*.15f,height+.42f,0},{width*.40f,.60f,depth*.45f},parent);
                if(seed%3==0)
                {
                    Box(7,{0,height+1.0f,0},{.06f,1.6f,.06f},parent);
                    Box(13,{0,height+1.83f,0},{.09f,.09f,.09f},parent);
                }
                const int floors=int(height/.80f);
                for(int floor=0;floor<floors;++floor)
                {
                    const float y=.45f+floor*.80f;
                    // Recessed dark window strips and lit panes on all faces.
                    for(int side=0;side<4;++side)
                    {
                        const bool front=side<2;
                        const float span=front?width:depth;
                        const int count=int(span/.47f);
                        for(int c=0;c<count;++c)
                        {
                            const int hash=seed+floor*19+c*7+side*11;
                            const int material=hash%5<2?11:(hash%4==0?10:9);
                            const float a=-span*.5f+(c+.5f)*span/count;
                            const float sign=side%2==0?-1.f:1.f;
                            Box(material,front?glm::vec3(a,y,sign*(depth*.5f+.025f)):
                                glm::vec3(sign*(width*.5f+.025f),y,a),
                                front?glm::vec3(.23f,.34f,.018f):glm::vec3(.018f,.34f,.23f),
                                parent);
                        }
                    }
                }
                // Vertical corner pilasters give building sides a readable edge.
                for(float edge : {-1.f,1.f})
                    Box(7,{edge*width*.49f,height*.5f,-depth*.5f-.025f},
                        {.075f,height,.05f},parent);
            }
        }
        for (size_t i=0;i<colours.size();++i)
        {
            Part part;
            part.colour=colours[i]; part.emissive=emission[i];
            if(!part.mesh.CreateMesh(vertices[i],indices[i])) return false;
            parts.push_back(std::move(part));
        }
        vertices.clear(); indices.clear();
        return true;
    }
private:
    std::vector<std::vector<Vertex>> vertices;
    std::vector<std::vector<unsigned int>> indices;
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
