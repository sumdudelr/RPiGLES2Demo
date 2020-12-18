#version 100
attribute vec3 Vert;
attribute vec3 Color;
attribute float Size;

varying vec3 color;

uniform mat4 mvp;

void main()
{
    color = Color;
    gl_PointSize = Size;
    gl_Position = mvp * vec4(Vert, 1.0);
}
