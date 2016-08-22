#version 330 core

uniform sampler2D diffuseSampler;

in vec3 vsPositionView;
in vec2 vsTextureCoord;
layout (location = 0) out vec4 fsPositionView;
layout (location = 1) out vec4 fsNormalView;
layout (location = 2) out vec4 fsDiffuse;

void main()
{
	fsDiffuse = texture(diffuseSampler, vsTextureCoord);
	fsPositionView = vec4(vsPositionView, fsDiffuse.a);
	fsNormalView = vec4(0.0, 0.0, 1.0, fsDiffuse.a);
}
