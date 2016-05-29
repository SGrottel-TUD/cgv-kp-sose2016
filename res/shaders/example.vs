#version 330 core

uniform mat4x4 projectionView;

layout(location = 0) in vec3 inPosition;

void main()
{
	gl_Position = projectionView * vec4(inPosition, 1);
}
