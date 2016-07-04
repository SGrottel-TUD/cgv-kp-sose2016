#version 330 core

layout(location = 0) in vec3 inPosition;

out vec2 vsTextureCoord;

void main()
{
	gl_Position = vec4(inPosition, 1.0);
	vsTextureCoord = (inPosition.xy + vec2(1.0, 1.0)) / 2.0;
}
