#version 330 core

uniform mat4 worldView;
uniform mat4 worldViewProjection;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTextureCoord;
out vec3 vsPositionView;
out vec3 vsNormalView;
out vec2 vsTextureCoord;

void main()
{
	gl_Position = worldViewProjection * vec4(inPosition, 1.0);
	vsPositionView = (worldView * vec4(inPosition, 1.0)).xyz;
	vsNormalView = (worldView * vec4(inNormal, 0.0)).xyz;
	vsTextureCoord = inTextureCoord;
}
