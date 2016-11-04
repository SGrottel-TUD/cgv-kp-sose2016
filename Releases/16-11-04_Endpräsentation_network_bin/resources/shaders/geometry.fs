#version 330 core

uniform sampler2D textureSampler;

in vec3 vsPositionView;
in vec3 vsNormalView;
in vec2 vsTextureCoord;

layout (location = 0) out vec3 fsPositionView;
layout (location = 1) out vec3 fsNormalView;
layout (location = 2) out vec3 fsDiffuse;


void main()
{
	fsPositionView = vsPositionView;
	fsNormalView = normalize(vsNormalView);
	fsDiffuse = texture(textureSampler, vsTextureCoord).rgb;
}
