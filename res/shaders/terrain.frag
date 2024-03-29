#version 330 core

in vec3 terrainColour;
in vec4 viewSpace;
in vec3 skyColour_v;

out vec4 outputColour;

uniform float fogDensity;
uniform float fogGradient;

float getFogModifier() {
	float 
		distance = length(viewSpace),
		fogModifier = exp(-pow((distance * fogDensity), fogGradient));
	
	return clamp(fogModifier, 0.0f, 1.0f);
}

void main(){
	outputColour = vec4(mix(skyColour_v, terrainColour, getFogModifier()), 1.0f);
}