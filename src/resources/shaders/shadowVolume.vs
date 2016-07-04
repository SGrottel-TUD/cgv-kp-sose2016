#version 330

uniform mat4 world;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 positionWorld;
out vec3 normalWorld;

void main()
{
	positionWorld = (world * vec4(position, 1.0)).xyz;
	normalWorld = (world * vec4(normal, 0.0)).xyz;
}
