#version 330 core

in float z;

out vec3 color;

void main()
{
	if (z == 0.0)
	{
		color = vec3(1.0);
	}
	else
	{
		color = vec3(0.0);
	}
}
