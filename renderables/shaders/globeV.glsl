#version 100
attribute vec3 Vert;
attribute vec3 Norm;
attribute vec2 Texc;

varying vec3 FragPos;
varying vec3 Normal;
varying vec2 TexCoord;

uniform mat4 model;
uniform mat4 vp;
uniform mat3 ipModel;

void main() {
        FragPos = vec3(model * vec4(Vert, 1.0));
        Normal = ipModel * Norm;
        TexCoord = Texc;
        
        // Multiplication reads right to left
        gl_Position = vp * model * vec4(Vert, 1.0);
}

