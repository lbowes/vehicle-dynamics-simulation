#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 2) in vec3 a_Normal;

out vec3 colour;
out vec4 viewSpace;
out vec3 skyColour_v;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 skyColour;
uniform vec3 sunDirection;
uniform vec3 bodyColour;

void main(){
	float lighting = max(dot(a_Normal, inverse(mat3(modelMatrix)) * sunDirection), 0.0f);
	colour = mix(skyColour * 0.1, bodyColour, lighting);
	skyColour_v = skyColour;

	viewSpace = viewMatrix * modelMatrix * vec4(a_Position, 1.0f);
	gl_Position = projectionMatrix * viewSpace;
}