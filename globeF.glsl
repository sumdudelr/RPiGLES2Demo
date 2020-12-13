#version 100
varying vec3 FragPos;
varying vec3 Normal;
varying vec2 TexCoord;

uniform vec3 lightPos;
uniform sampler2D texture1;
uniform sampler2D texture2;

float LightIntensity(vec3 normal, vec3 lightPos, vec3 fragPos)
{
    vec3 lightDir = normalize(lightPos);
    float diffuse = max(dot(normal, lightDir), 0.0) * 0.7;
    vec3 viewDir = normalize(fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), 32.0) * 0.5;
    float ambient = 0.1;
    
    return diffuse + specular + ambient;
}

vec2 ComputeTextureCoordinates(vec3 normal)
{
    return vec2(-atan(normal.y, normal.x) * (1.0/(2.0*radians(180.0))) + 0.5, asin(normal.z) * (1.0/radians(180.0)) + 0.5);
}

void main()
{
    vec3 lightIntensity = vec3(LightIntensity(Normal, lightPos, FragPos));
    gl_FragColor = texture2D(texture1, TexCoord) * vec4(lightIntensity, 1.0);
    //~ gl_FragColor = texture2D(texture1, ComputeTextureCoordinates(Normal));
    //~ gl_FragColor = mix(texture2D(texture1, TexCoord), texture2D(texture2, TexCoord), 0.2);
    //~ gl_FragColor = texture2D(texture1, TexCoord);
    //~ gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0);
}
