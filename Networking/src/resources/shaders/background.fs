#version 330 core

uniform sampler2D textureSampler;

in vec2 vsTextureCoord;

out vec4 final_color;

void main()
{
	final_color = vec4(texture(textureSampler, vsTextureCoord).rgb, 1.0);
}
