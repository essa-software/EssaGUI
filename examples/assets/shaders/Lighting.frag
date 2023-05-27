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
uniform vec4 diffuseColor; // Already linear!
uniform vec4 emissionColor; // Already linear!

vec3 clampColor(vec3 color) {
    return clamp(color, vec3(0.0), vec3(1.0));
}

float GAMMA = 2.2;

vec3 linear_to_gamma(vec3 c) {
    // return c;
    return vec3(pow(c.r, 1.0/GAMMA), pow(c.g, 1.0/GAMMA), pow(c.b, 1.0/GAMMA));
}

vec3 gamma_to_linear(vec3 c) {
    // return c;
    return vec3(pow(c.r, GAMMA), pow(c.g, GAMMA), pow(c.b, GAMMA));
}

void main()
{
    vec3 lightPosVS = vec3(inverse(viewMatrix * modelMatrix) * vec4(lightPosition, 1));
    // TODO: Make all the factors configurable and not picked up from nowhere
    const float AmbientFactor = 0.3;

    vec3 diffuseColorL = diffuseColor.rgb;
    vec3 lightColorL = gamma_to_linear(lightColor.rgb);

    vec3 ambient = clampColor(AmbientFactor * diffuseColorL * lightColorL);
  	
    // diffuse 
    vec3 norm = normalize(f_normal);
    vec3 lightDir = normalize(lightPosVS - f_position);
    float diff = (dot(norm, lightDir) + 1) / 2;
    vec3 diffuse = clampColor(diff * diffuseColorL * lightColorL);
    
    vec4 source = textureSet ? texture2D(texture, f_texCoord) : f_color;
    vec3 sourceL = gamma_to_linear(source.rgb);

    vec3 result = (ambient + diffuse) * sourceL;
    result += emissionColor.rgb;
    gl_FragColor = vec4(linear_to_gamma(result), source.a);
}
