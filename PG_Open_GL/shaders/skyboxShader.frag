#version 410 core

in vec3 textureCoordinates;
out vec4 color;

uniform samplerCube skybox;
uniform float fogDensity;

void main()
{
    vec4 skyboxColor = texture(skybox, textureCoordinates, 2.5);
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

    float factor = clamp(1.0 - abs(textureCoordinates.y), 0.0, 1.0);
    

    factor = pow(factor, 1.2) * min(fogDensity * 40.0, 1.0);
    
    color = mix(skyboxColor, fogColor, factor);
}