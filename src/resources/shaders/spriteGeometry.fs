#version 330 core

uniform vec2 material;
uniform sampler2D diffuseSampler;
uniform sampler2D depthOffsetSampler;

in vec3 vsPositionView;
in vec2 vsTextureCoord;
layout (location = 0) out vec4 fsPositionView;
layout (location = 1) out vec4 fsNormalView;
layout (location = 2) out vec4 fsDiffuse;
layout (location = 3) out vec2 fsMaterial;

void main()
{
	vec4 depthOffset = texture(depthOffsetSampler, vsTextureCoord);
	fsDiffuse = texture(diffuseSampler, vsTextureCoord);
	fsPositionView = vec4(vsPositionView, fsDiffuse.a);
	fsPositionView.z += depthOffset.x - 0.5;
	fsNormalView = vec4(0.0, 1.0, 0.0, fsDiffuse.a);
	fsMaterial = material;
}
