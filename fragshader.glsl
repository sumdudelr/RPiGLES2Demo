#version 100
varying vec3 Normal;
varying vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

vec2 ComputeTextureCoordinates(vec3 normal)
{
    return vec2(atan(normal.y, normal.x) * (1.0/(2.0*radians(180.0))) + 0.5, asin(normal.z) * (1.0/radians(180.0)) + 0.5);
}

void main()
{
    //~ gl_FragColor = texture2D(texture1, ComputeTextureCoordinates(Normal));
    //~ gl_FragColor = mix(texture2D(texture1, TexCoord), texture2D(texture2, TexCoord), 0.2);
    gl_FragColor = texture2D(texture1, TexCoord);
    //~ gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0);
}
