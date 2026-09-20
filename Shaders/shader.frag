#version 330 core

out vec4 colour;
uniform vec3 objectColour;

void main()
{
    colour = vec4(objectColour, 1.0);
}
