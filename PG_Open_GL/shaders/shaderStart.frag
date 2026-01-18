#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

// Directional light(the moving sun)
uniform vec3 lightDir;
uniform vec3 lightColor;

// Point Light (the bonfire)
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;
uniform int pointLightOn;

// Spotlight (Flashlight)
uniform int flashlightOn;
uniform vec3 spotLightDir; // In eye space: vec3(0.0, 0.0, -1.0)
uniform float spotLightCutOff;
uniform float spotLightOuterCutOff;

// Textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

uniform float textureRepeat;
uniform float hasAlpha;
uniform float fogDensity;

//Lighting coefficients
float ambientStrength = 0.2f;
float specularStrength = 0.5f;
float shininess = 32.0f;

//Attenuation constants
float constant = 1.0f;
float linear = 0.045f;
float quadratic = 0.0075f;

//Fog color 
vec3 fogColor = vec3(0.5f, 0.5f, 0.5f);
vec3 computeFog(vec3 color)
{
    float fragmentDistance = length(fPosEye.xyz);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
    fogFactor = clamp(fogFactor, 0.0f, 1.0f);
    

    return mix(fogColor, color, fogFactor);
}

float computeShadow() {
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    normalizedCoords = normalizedCoords * 0.5 + 0.5;
    
    if(normalizedCoords.z > 1.0) return 0.0;
    
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
    float currentDepth = normalizedCoords.z;
    float bias = 0.005f;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

void main() 
{
    vec2 repeatedCoords = fTexCoords * textureRepeat;
    vec4 colorFromTexture = texture(diffuseTexture, repeatedCoords);
    
    if(hasAlpha > 0.5f && colorFromTexture.a < 0.9)
        discard;

    vec3 normalEye = normalize(fNormal);
    vec3 viewDirN = normalize(-fPosEye.xyz);
    vec3 finalLight = vec3(0.0);

    if (flashlightOn == 1) {
        // FLASHLIGHT LOGIC
        // In Eye Space, pozitia luminii este (0,0,0) deoarece este atasata camerei
		vec3 handOffset = vec3(0.5f, -0.3f, 0.0f);
        vec3 lightDirN = normalize(handOffset -fPosEye.xyz); 
        float theta = dot(lightDirN, normalize(-spotLightDir));
        float epsilon = spotLightCutOff - spotLightOuterCutOff;
        float intensity = clamp((theta - spotLightOuterCutOff) / epsilon, 0.0, 1.0);

        // Diffuse
        float diff = max(dot(normalEye, lightDirN), 0.0f);
        // Specular
        vec3 reflection = reflect(-lightDirN, normalEye);
        float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);

        vec3 spotAmbient = ambientStrength * vec3(1.0) * colorFromTexture.rgb;
        vec3 spotDiffuse = diff * vec3(1.0) * colorFromTexture.rgb;
        vec3 spotSpecular = specularStrength * specCoeff * texture(specularTexture, repeatedCoords).rgb;

        finalLight = (spotAmbient + spotDiffuse + spotSpecular) * intensity;
    } 
    else {
        // Directional Light Logic
        float shadow = computeShadow();
        vec3 lightDirN = normalize(lightDir);
        
        vec3 ambient = ambientStrength * lightColor * colorFromTexture.rgb;
        vec3 diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor * colorFromTexture.rgb;
        
        vec3 reflection = reflect(-lightDirN, normalEye);
        float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
        vec3 specular = specularStrength * specCoeff * lightColor * texture(specularTexture, repeatedCoords).rgb;

        finalLight = ambient + (1.0 - shadow) * (diffuse + specular);
    }

    // Point Light (in diamond ore)
    if (pointLightOn == 1) {
        vec3 pLightDirN = normalize(pointLightPos - fPosEye.xyz);
        float dist = length(pointLightPos - fPosEye.xyz);
        float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));

        vec3 pDiffuse = att * max(dot(normalEye, pLightDirN), 0.0f) * pointLightColor * colorFromTexture.rgb;
        finalLight += pDiffuse;
    }

    vec3 finalColor = min(finalLight, 1.0f);
    fColor = vec4(computeFog(finalColor), 1.0f);
}