#version 460 core
layout (location = 0) in vec3 pos;   

#include "../Common/Camera.frag"

void main()
{
    gl_Position = VP * vec4(pos.x, pos.y, pos.z,1.0);
}  