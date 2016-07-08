#version 330 core

uniform sampler2D textureSampler;

in vec2 vsTextureCoord;
out vec3 fsColor;

void main()
{
	fsColor = texture(textureSampler, vsTextureCoord).rgb;
}
