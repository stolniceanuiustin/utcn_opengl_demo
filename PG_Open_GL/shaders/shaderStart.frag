#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

// Lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

// Point Light
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;
uniform int pointLightOn;

// Texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

uniform float textureRepeat;
uniform float hasAlpha;
uniform mat4 view;
uniform float fogDensity;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

float constant = 1.0f;
float linear = 0.045f;
float quadratic = 0.0075f;

float computeFog()
{
    float fragmentDistance = length(fPosEye.xyz);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

    return clamp(fogFactor, 0.0f, 1.0f);
}

float computeShadow() {
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    normalizedCoords = normalizedCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
    float currentDepth = normalizedCoords.z;
    float bias = 0.005f;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    if(normalizedCoords.z > 1.0) return 0.0;
    return shadow;
}

void computeLightComponents()
{       
    vec3 cameraPosEye = vec3(0.0f);
    vec3 normalEye = normalize(fNormal);    
    vec3 lightDirN = normalize(lightDir);
    vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
        
    ambient = ambientStrength * lightColor;
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
    
    vec3 reflection = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
    specular = specularStrength * specCoeff * lightColor;
}

void main() 
{
    vec2 repeatedCoords = fTexCoords * textureRepeat;
    vec4 colorFromTexture = texture(diffuseTexture, repeatedCoords);
    
    if(hasAlpha > 0.5f)
        if(colorFromTexture.a < 0.9)
            discard;

    float shadow = computeShadow();
    computeLightComponents();
    
    vec3 baseAmbient = ambient * colorFromTexture.rgb;
    vec3 baseDiffuse = (1.0f - shadow) * diffuse * colorFromTexture.rgb;
    vec3 baseSpecular = (1.0f - shadow) * specular * texture(specularTexture, repeatedCoords).rgb;

    vec3 finalPointLight = vec3(0.0f);

    if (pointLightOn == 1) {
        vec3 normalEye = normalize(fNormal);
        vec3 viewDirN = normalize(-fPosEye.xyz);
        vec3 pLightDirN = normalize(pointLightPos - fPosEye.xyz);
        float dist = length(pointLightPos - fPosEye.xyz);
        float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));

        vec3 pAmbient = att * ambientStrength * pointLightColor * colorFromTexture.rgb;
        vec3 pDiffuse = att * max(dot(normalEye, pLightDirN), 0.0f) * pointLightColor * colorFromTexture.rgb;
        vec3 pReflection = reflect(-pLightDirN, normalEye);
        float pSpecCoeff = pow(max(dot(viewDirN, pReflection), 0.0f), shininess);
        vec3 pSpecular = att * specularStrength * pSpecCoeff * pointLightColor * texture(specularTexture, repeatedCoords).rgb;
        
        finalPointLight = pAmbient + pDiffuse + pSpecular;
    }

    vec3 color = min(baseAmbient + baseDiffuse + baseSpecular + finalPointLight, 1.0f);

    float fogFactor = computeFog();
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f); 
    
    fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);
}