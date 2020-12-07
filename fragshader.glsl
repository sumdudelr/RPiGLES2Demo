#version 100
varying vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    //~ gl_FragColor = mix(texture2D(texture1, TexCoord), texture2D(texture2, TexCoord), 0.2);
    gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0);
}
