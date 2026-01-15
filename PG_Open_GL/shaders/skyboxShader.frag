#version 410 core

in vec3 textureCoordinates;
out vec4 color;

uniform samplerCube skybox;
uniform float fogDensity;
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
void main()
{
	// Deoarece skybox-ul se afla la o distanta infinita(teoretic), nu putem foloosi distanta fata de camera
	// Folosim coordonata de pe axa Y pentru a determina unde sa aplicam ceata
	
	// Citim culoarea din cube map 
    vec4 skyboxColor = texture(skybox, textureCoordinates, 2.5 /*LOD - Level of detail*/);

	//la orizont vom avea intensitate maxima, la polii cerului vom avea intensitate minima
    float factor = clamp(1.0 - abs(textureCoordinates.y), 0.0, 1.0);
    
	
    factor = pow(factor, 1.2/* controleaza cat de lin se face tranzitia*/) * min(fogDensity * 40.0 /*avem nevoie de o densitate mare pe cer*/, 1.0);
    
    color = mix(skyboxColor, fogColor, factor);
}