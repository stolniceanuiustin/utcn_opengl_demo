#version 410 core

in vec2 texCoords;

uniform sampler2D diffuseTexture;
uniform float hasAlpha;

void main()
{
    if (hasAlpha > 0.5) {
        float alpha = texture(diffuseTexture, texCoords).a;
        if(alpha < 0.1) {
            discard; 
        }
    }

}