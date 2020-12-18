#version 100
attribute vec3 Vert;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(Vert, 1.0);
}
