#version 330

uniform mat4 worldViewProjection;
uniform vec2 textureCoordOffset;

layout (location = 0) in vec3 inPosition;
layout (location = 2) in vec2 inTextureCoord;
out vec2 vsTextureCoord;

void main()
{
	gl_Position = worldViewProjection * vec4(inPosition, 1.0);
	vsTextureCoord = inTextureCoord + textureCoordOffset;
}
