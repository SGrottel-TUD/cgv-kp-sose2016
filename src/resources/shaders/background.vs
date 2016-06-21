#version 330 core

layout(location = 0) in vec2 inPosition;

out vec2 vsTextureCoord;

void main()
{
	gl_Position = vec4(inPosition, 0.0, 1.0);
	vsTextureCoord = (inPosition + vec2(1.0, 1.0)) / 2.0;
}
