#version 100
attribute vec3 Vert;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(Vert, 1.0);
}
