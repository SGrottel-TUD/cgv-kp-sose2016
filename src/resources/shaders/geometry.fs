#version 330 core

uniform vec3 ambientLight;
uniform vec2 material;

in vec3 vsPositionWorld;
in vec3 vsNormalWorld;
in vec2 vsTextureCoord;

layout(location = 0) out vec3 fsPositionWorld;
layout(location = 1) out vec3 fsNormalWorld;
layout(location = 2) out vec3 fsDiffuse;
layout(location = 3) out vec2 fsMaterial;
layout(location = 4) out vec3 fsAmbient;

void main()
{
	fsPositionWorld = vsPositionWorld;
	fsNormalWorld = vsNormalWorld;
	fsDiffuse = vec3(vsTextureCoord, 1);
	fsMaterial = material;
	fsAmbient = fsDiffuse * ambientLight;
}
