#version 100
attribute vec3 Vert;
attribute vec3 Norm;

varying vec3 FragPos;
varying vec3 Normal;

uniform mat4 vp;
uniform mat3 ipModel;

void main() {
        mat4 model = mat4(1.0);
        FragPos = vec3(model * vec4(Vert, 1.0));
        Normal = ipModel * Norm;
        
        // Multiplication reads right to left
        gl_Position = vp * model * vec4(Vert, 1.0);
}

