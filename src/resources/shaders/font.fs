#version 330 core

in float z;

out vec3 color;

void main()
{
	color = vec3(1, 0, 0) * (1 + z* 5);
}
