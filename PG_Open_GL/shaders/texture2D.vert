#version 410 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform vec2 offset;
uniform float scale;
uniform float rotation_angle;
uniform vec2 position;

out vec2 TexCoords;

void main()
{
    // Apply scaling
    vec2 scaledPos = aPos * scale;

    // Apply translation (base position + bobbing offset)
    gl_Position = vec4(scaledPos.x + position.x + offset.x, scaledPos.y + position.y + offset.y, 0.0, 1.0);

    // Rotate texture coordinates
    float cosAngle = cos(rotation_angle);
    float sinAngle = sin(rotation_angle);
    mat2 rotationMatrix = mat2(cosAngle, -sinAngle, sinAngle, cosAngle);
    TexCoords = rotationMatrix * (aTexCoords - 0.5) + 0.5;
}