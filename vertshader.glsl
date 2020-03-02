#version 100
attribute vec3 aPos;
attribute vec2 aTexCoord;

varying vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
        // Multiplication reads right to left
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
}

