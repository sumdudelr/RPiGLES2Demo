#version 100
varying vec2 TexCoord;

uniform sampler2D texture;

void main()
{

    vec3 textColor = vec3(1.0, 1.0, 1.0); // white
    float text = texture2D(texture, TexCoord).a;
    float alpha = text + 0.5;
    gl_FragColor = vec4(textColor * vec3(text), alpha);
}
