#version 330 core

layout(location = 0) in vec2 inPosition;

uniform ivec2 textureSize;
uniform ivec2 screenSize;

out vec2 vsTextureCoord;

void main()
{
	gl_Position = vec4(inPosition, 0.0, 1.0);
	vec2 aspects = (1.0 * screenSize) / textureSize;
	vsTextureCoord = ((inPosition + vec2(1.0, 1.0)) / 2.0)*aspects;
}
