#version 330 core

layout(location = 0) in vec3 inPosition;

uniform ivec2 textureSize;
uniform ivec2 screenSize;

out vec2 vsTextureCoord;

void main()
{
	gl_Position = vec4(inPosition, 1.0);
	vec2 aspects = (1.0 * screenSize) / textureSize;
	vsTextureCoord = (inPosition.xy + vec2(1.0)) * 0.5 * aspects;
}
