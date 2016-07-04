#version 330

layout (location = 0) in vec3 inPosition;

out vec2 vsTextureCoord;

void main()
{
	gl_Position = vec4(inPosition, 1.0);
	vsTextureCoord = (gl_Position.xy / gl_Position.w + vec2(1.0)) * 0.5;
}
