#version 330 core
layout (location = 0) in vec2 vertex_pos;
layout (location = 1) in vec2 texture_coord;
out vec2 outCoord;
void main(void)
{
    gl_Position = vec4(vertex_pos, 1.0 ,1.0);
    outCoord=texture_coord;
}
