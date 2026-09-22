#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 worldPosition;
out vec3 worldNormal;
out vec2 textureCoordinate;

void main()
{
    vec4 world = model * vec4(pos, 1.0);
    worldPosition = world.xyz;
    worldNormal = mat3(transpose(inverse(model))) * normal;
    textureCoordinate = texCoord;
    gl_Position = projection * view * world;
}
