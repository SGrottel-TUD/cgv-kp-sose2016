#version 330 core

uniform mat4x4 world;
uniform mat4x4 worldViewProjection;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoord;

out vec3 vsPositionWorld;
out vec3 vsNormalWorld;
out vec2 vsTextureCoord;

void main()
{
	vsPositionWorld = (world * vec4(inPosition, 1)).xyz;
	gl_Position = worldViewProjection * vec4(inPosition, 1);
	vsNormalWorld = (world * vec4(inNormal, 0)).xyz;
	vsTextureCoord = inTextureCoord;
}
