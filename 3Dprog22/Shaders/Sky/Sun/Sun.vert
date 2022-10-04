#version 460 core
layout (location = 0) in vec3 pos0;   
layout (location = 1) in vec2 texCoords0;

uniform mat4 MVP;

out vec2 texCoords;

void main()
{
    texCoords = texCoords0;

    gl_Position = (MVP * vec4(pos0.x, pos0.y, pos0.z, 1.0)).xyww;
}  