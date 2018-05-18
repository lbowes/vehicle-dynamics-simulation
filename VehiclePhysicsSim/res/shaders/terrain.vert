#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Colour;

out vec3 terrainColour;
out vec4 viewSpace;
out vec3 skyColour_v;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 sunDirection;
uniform vec3 skyColour;

void main(){
	float lighting = max(dot(a_Normal, sunDirection), 0.0f);
	terrainColour = mix(skyColour * 0.1f, a_Colour, lighting);
	skyColour_v = skyColour;

	viewSpace = viewMatrix * modelMatrix * vec4(a_Position, 1.0f);
	gl_Position = projectionMatrix * viewSpace;
}