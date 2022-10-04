#version 460 core
layout (location = 0) in vec2 pos0;   
layout (location = 1) in vec2 texCoords0;

uniform vec2 pos2D;
uniform vec2 scale2D;

out vec2 texCoords;

void main()
{
    texCoords = texCoords0;
    
    vec2 finalPos =  scale2D * pos0 + pos2D;
    gl_Position = vec4(finalPos, 0.f, 1.0);
}  