#version 330 core
layout (location = 0) in vec3 a_Position;

out vec3 vertexPosition;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main() {
	vertexPosition = a_Position;
	gl_Position = projectionMatrix * viewMatrix * vec4(a_Position, 1.0f);
}