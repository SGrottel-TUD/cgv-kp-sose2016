#version 330 core

uniform vec2 material;
uniform sampler2D textureSampler;

in vec3 vsPositionView;
in vec3 vsNormalView;
in vec2 vsTextureCoord;

layout(location = 0) out vec3 fsPositionView;
layout(location = 1) out vec3 fsNormalView;
layout(location = 2) out vec3 fsDiffuse;
layout(location = 3) out vec2 fsMaterial;

void main()
{
	fsPositionView = vsPositionView;
	fsNormalView = normalize(vsNormalView);
	vec4 tex = texture(textureSampler, vsTextureCoord);
	fsDiffuse = vec3(tex) * tex.a;
	fsMaterial = material;
}
