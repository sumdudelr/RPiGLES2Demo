#version 100
attribute vec2 Vert;
attribute vec2 Texc;

varying vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool isBox;

void main()
{
    TexCoord = Texc;
    if (isBox)
    {
        // Draw box behind text
        gl_Position = projection * model * vec4(Vert, 0.5, 1.0);
    }
    else
    {
        gl_Position = projection * model * vec4(Vert, 0.0, 1.0);
    }
}
