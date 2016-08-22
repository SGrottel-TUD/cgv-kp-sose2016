#version 330

uniform mat4 projection = mat4(1);

layout (location = 0) in vec3 inPosition;
noperspective out vec2 vsTextureCoord;

void main()
{
	gl_Position = projection * vec4(inPosition, 1.0);
	vsTextureCoord = gl_Position.xy * (0.5 / gl_Position.w) + 0.5;
}
