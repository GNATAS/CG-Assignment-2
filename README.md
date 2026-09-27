# Assignment 2 — Future Me

An 800 × 600 C++17 / OpenGL 3.3 scene of a programmer working in a high-rise
condo at night. The person, desk, monitors, keyboard and mouse remain the
pair's original models. Other furniture, curtains and city
buildings use external OBJ models. The room and window are drawn
from vertices in `src/Libs/Environment.h`; city window lights use the fragment shader.

## Build and run

From this directory on the configured Windows machine:

```
cmake --build --preset build-win-2026-debug
./build/win-msvc-2026/Debug/OpenGLStarter.exe
```

The source, shaders, texture and model files under `src/`, `Shaders/` and
`model/Assets/` are needed to build or run the scene. `CMakeLists.txt`,
`CMakePresets.json` and `vcpkg.json` describe the build dependencies.
Escape closes the program.

The latest 800 × 600 preview is `build/city-obj-preview.png`.
The user's curtain export is triangulated in `Models/curtain/curtain.obj`.
The metal curtain pole, end caps and wall brackets use triangulated cylinders
with a specular material in `src/Libs/Environment.h`.
The bedroom has an OBJ wall bookshelf above the headboard, books and a small
potted plant, with more than 0.32 m clearance from the curtain. A round oak
side table sits beside the computer desk, with its reading lamp aimed toward
the seated person and a small plant on the tabletop. A wall clock sits
above the headboard shelf. The incorrect floor-mounted wall shelf is removed.
The white wall art and old tall bookshelf remain removed. The original
workstation and interior layout are unchanged by the skyline update.
The city uses three external triangulated OBJ building models in 26 placements,
with varied heights, visible rooftops, night window lights and distance haze.
Black source facade textures are replaced with slate-blue facade materials;
the original full PBR appearance is not reproduced. A captured frame
reported GL error 0, but the earlier on-screen flicker has not been verified
fixed.

## External assets and submission

See `model/Assets/EXTERNAL_MODEL_CREDITS.md` for the model names, creators,
source links and license information. Runtime OBJ, MTL and texture files are
tracked with the assignment so teammates can obtain the complete scene.
External assets retain their original licenses and attribution requirements.

Before Google Classroom submission, use the confirmed student IDs to prepare
the required `Assignment2_studentID.cpp`, accompanying shaders/assets and
`Assignment2_studentID_pairID.png` or `.jpg`. Include both members' names and
IDs plus reference/asset credits in the submission comment. The stated
deadline is 23:59, Monday 19 October 2026.
