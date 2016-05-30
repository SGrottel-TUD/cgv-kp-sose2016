#version 330 core

uniform mat4x4 worldViewProjection;

layout(location = 0) in vec3 inPosition;

void main()
{
	gl_Position = worldViewProjection * vec4(inPosition, 1);
}
