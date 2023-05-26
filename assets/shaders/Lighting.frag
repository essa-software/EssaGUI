#version 410 core
in vec3 f_position;
in vec4 f_color;
in vec2 f_texCoord;
in vec3 f_normal;
uniform sampler2D texture;
uniform bool textureSet;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

uniform vec3 lightPosition;
uniform vec4 lightColor;

// uniform vec4 ambientColor; // Unused
uniform vec4 diffuseColor;
uniform vec4 emissionColor;

void main()
{
    vec3 lightPosVS = vec3(inverse(viewMatrix * modelMatrix) * vec4(lightPosition, 1));
    // TODO: Make all the factors configurable and not picked up from nowhere
    const float AmbientFactor = 0.3;

    vec4 ambient = AmbientFactor * diffuseColor * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(f_normal);
    vec3 lightDir = normalize(lightPosVS - f_position);
    float diff = dot(norm, lightDir);
    vec4 diffuse = diff * diffuseColor * lightColor;
    
    vec4 source = textureSet ? texture2D(texture, f_texCoord) : f_color;

    vec4 result = (ambient + diffuse) * source;
    result += emissionColor;
    gl_FragColor = vec4(result.rgb, source.a);
}
