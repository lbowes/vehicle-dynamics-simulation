#version 330 core

in vec3 vertexPosition;

out vec4 colour;

uniform vec3 skyColour;
uniform vec3 sunColour;
uniform vec3 sunDir;

void main() {
	const float 
		b1 = 0.0005f,       //0.0005f 
		b2 = 0.0005f,       //0.0005f
		b3 = 0.8f,          //0.8f
		b2Intensity = 0.9f;	//0.7f
	
	float 
		intensity = 0.0f,
		betweenSun = 1.0f - max(dot(normalize(vertexPosition), normalize(sunDir)), 0.0f);    

	if(betweenSun < b1)
		intensity = 1.0f;
	else if(betweenSun > b1 && betweenSun < b1 + b2)
		intensity = b2Intensity + 1.0f - (betweenSun - b1) / b2;
	else if(betweenSun > b1 + b2 && betweenSun < b1 + b2 + b3)
		intensity = (1.0f - (betweenSun - b2 - b2) / b3) * b2Intensity;

   intensity = clamp(intensity, 0.0f, 1.0f);

    colour = vec4(mix(skyColour, sunColour, intensity), 1.0f);
}