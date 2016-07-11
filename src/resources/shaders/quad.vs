#version 330

uniform mat4 projection = mat4(1);

layout (location = 0) in vec3 inPosition;
out vec2 vsTextureCoord;

void main()
{
	gl_Position = projection * vec4(inPosition, 1.0);
	vsTextureCoord = (gl_Position.xy / gl_Position.w + vec2(1.0)) * 0.5;
}
