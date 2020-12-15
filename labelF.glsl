#version 100
varying vec2 TexCoord;

uniform sampler2D texture;

void main()
{

    vec3 textColor = vec3(1.0, 1.0, 1.0); // white
    gl_FragColor = vec4(textColor * texture2D(texture, TexCoord).aaa, 1.0);
}
